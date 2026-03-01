/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:35:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/03/01 14:34:11 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** wait_dongle
** Blocks the calling coder under dongle->lock until all three conditions
** are satisfied simultaneously:
**   1. coder is at the head of dongle->waitlist (lowest scheduler key)
**   2. dongle->in_use is false (no other coder currently holds it)
**   3. dongle cooldown has expired (get_timestamp >= time_end_cooldown)
** Uses timedwait when only the cooldown is pending (in_use == false) so
** the thread auto-wakes at time_end_cooldown. Falls back to cond_wait
** when in_use == true since only a broadcast from release_dongle can
** unblock the waiting thread. Exits immediately if stop_sim is set.
** Must be called with dongle->lock already held.
**
** @param coder   Pointer to the waiting coder.
** @param dongle  Pointer to the dongle being waited on.
*/
static void	wait_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;
	uint64_t		wake_at;

	while (heap_peek(&dongle->waitlist).coder_id != coder->id || dongle->in_use
		|| get_timestamp() < dongle->time_end_cooldown)
	{
		if (get_stop_sim(coder->sim))
			break ;
		wake_at = dongle->time_end_cooldown;
		if (!dongle->in_use && get_timestamp() < wake_at)
		{
			ts.tv_sec = wake_at / 1000;
			ts.tv_nsec = (wake_at % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->lock);
	}
}

/*
** take_dongle
** Acquires exclusive access to a dongle for the requesting coder.
** Pushes a scheduler node (coder_id, key) into the dongle waitlist heap,
** then delegates to wait_dongle() which blocks under dongle->lock until:
** the coder is at the head of the waitlist (lowest key), dongle->in_use
** is false, AND the cooldown has expired. On success pops the node,
** sets dongle->in_use = true and returns true. If stop_sim is raised
** while waiting, unlocks and returns false without marking in_use.
**
** @param coder   Pointer to the requesting coder.
** @param dongle  Pointer to the target dongle.
** @param key     Scheduler sort key (arrival time for FIFO, deadline for EDF).
** @return        true if dongle was acquired, false if stop_sim was set.
*/
static bool	take_dongle(t_coder *coder, t_dongle *dongle, uint64_t key)
{
	t_heap_node		node;

	node.coder_id = coder->id;
	node.key = key;
	pthread_mutex_lock(&dongle->lock);
	heap_push(&dongle->waitlist, node);
	wait_dongle(coder, dongle);
	if (get_stop_sim(coder->sim))
		return (pthread_mutex_unlock(&dongle->lock), false);
	heap_pop(&dongle->waitlist);
	dongle->in_use = true;
	pthread_mutex_unlock(&dongle->lock);
	return (true);
}

/*
** release_dongle
** Releases a dongle and starts its cooldown period.
** Acquires dongle->lock, writes time_end_cooldown = now + time_cooldown,
** then broadcasts on dongle->cond so all waiting coders re-evaluate.
** The broadcast is issued under the lock to prevent any waiter from
** missing it between reading the condition and calling cond_wait.
**
** @param coder   Pointer to the coder releasing the dongle.
** @param dongle  Pointer to the dongle being released.
*/
void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->in_use = false;
	dongle->time_end_cooldown = get_timestamp() + coder->sim->time_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

/*
** wake_all_dongles
** Broadcasts on every dongle's condition variable so that threads blocked
** inside take_dongle (on cond_wait or cond_timedwait) wake up immediately,
** re-check stop_sim and exit without waiting for the next natural wakeup.
** Called by the monitor thread right after setting stop_sim to true.
**
** @param sim  Pointer to the simulation structure.
*/
void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->tab_dongles[i].lock);
		pthread_cond_broadcast(&sim->tab_dongles[i].cond);
		pthread_mutex_unlock(&sim->tab_dongles[i].lock);
		i++;
	}
}

/*
** acquire_dongles
** Acquires both dongles required for a compile cycle.
** If nb_coders == 1, left and right point to the same dongle: compiling
** is impossible so the function spins on stop_sim (100 µs sleep) until
** the monitor fires the burnout, then returns false.
** Otherwise: computes a single scheduler key, checks stop_sim, takes
** left dongle (logs LOG_TAKEN on success), then takes right dongle (logs
** LOG_TAKEN on success). If either take fails (stop_sim set), any already
** acquired dongle is released before returning false.
** Using one key for both takes ensures consistent priority ordering
** across FIFO and EDF schedulers.
**
** @param coder  Pointer to the coder requesting both dongles.
** @return       true if both dongles acquired, false if stop_sim was set.
*/
bool	acquire_dongles(t_coder *coder)
{
	uint64_t	key;

	if (coder->sim->nb_coders == 1)
	{
		while (!get_stop_sim(coder->sim))
			usleep(100);
		return (false);
	}
	key = scheduler(coder);
	if (get_stop_sim(coder->sim))
		return (false);
	if (!take_dongle(coder, coder->left_dongle, key))
		return (false);
	log_message(coder, LOG_TAKEN);
	if (!take_dongle(coder, coder->right_dongle, key))
		return (release_dongle(coder, coder->left_dongle), false);
	log_message(coder, LOG_TAKEN);
	if (get_stop_sim(coder->sim))
	{
		release_dongle(coder, coder->right_dongle);
		release_dongle(coder, coder->left_dongle);
		return (false);
	}
	return (true);
}
