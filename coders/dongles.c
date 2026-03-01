/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:35:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:42:42 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** take_dongle
** Blocks until coder is granted exclusive access to dongle.
** Pushes a scheduler node (coder_id, key) into the dongle waitlist heap,
** then waits under dongle->lock until: the coder is at the head of the
** waitlist (lowest key) AND the cooldown has expired. Uses timedwait when
** a cooldown is active so the thread auto-wakes at the cooldown end time.
** time_end_cooldown is read directly (not via getter) because the lock is
** already held — calling the getter would deadlock.
**
** @param coder   Pointer to the requesting coder.
** @param dongle  Pointer to the target dongle.
** @param key     Scheduler sort key (arrival time for FIFO, deadline for EDF).
*/
static void	take_dongle(t_coder *coder, t_dongle *dongle, uint64_t key)
{
	t_heap_node		node;
	struct timespec	ts;
	uint64_t		end_cd;

	node.coder_id = coder->id;
	node.key = key;
	pthread_mutex_lock(&dongle->lock);
	heap_push(&dongle->waitlist, node);
	while (heap_peek(&dongle->waitlist).coder_id != coder->id
		|| get_timestamp() < dongle->time_end_cooldown)
	{
		end_cd = dongle->time_end_cooldown;
		if (get_timestamp() < end_cd)
		{
			ts.tv_sec = end_cd / 1000;
			ts.tv_nsec = (end_cd % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->lock);
	}
	heap_pop(&dongle->waitlist);
	pthread_mutex_unlock(&dongle->lock);
}

/*
** scheduler
** Returns the sort key used to order coder requests in the dongle waitlist.
** FIFO (priority == 0): key = current timestamp at request time.
** EDF  (priority == 1): key = compile_start + time_burnout (earliest deadline).
** The same key is used for both left and right dongle acquisitions in a
** single compile cycle to prevent priority inversion between the two takes.
**
** @param coder  Pointer to the requesting coder.
** @return       Sort key as uint64_t milliseconds.
*/
uint64_t	scheduler(t_coder *coder)
{
	if (coder->sim->priority)
		return (coder->compile_start + coder->sim->time_burnout);
	return (get_timestamp());
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
** If there is only one coder, left and right dongles are the same object:
** compiling is impossible and the coder must burn out. In that case the
** function spins on stop_sim (sleeping 100 µs per iteration) until the
** monitor detects the burnout and sets stop_sim, then returns false.
** Otherwise computes a single scheduler key, then interleaves stop_sim
** checks with the two takes: checks stop_sim → takes left dongle →
** checks stop_sim → logs LOG_TAKEN (blue) → checks stop_sim → takes
** right dongle → checks stop_sim → logs LOG_TAKEN (blue).
** Using the same key for both ensures consistent priority ordering across
** FIFO and EDF schedulers.
**
** @param coder  Pointer to the coder requesting both dongles.
** @return       true on success, false if stop_sim is set at any check point.
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
	take_dongle(coder, coder->left_dongle, key);
	if (get_stop_sim(coder->sim))
		return (false);
	log_message(coder, LOG_TAKEN);
	if (get_stop_sim(coder->sim))
		return (false);
	take_dongle(coder, coder->right_dongle, key);
	if (get_stop_sim(coder->sim))
		return (false);
	log_message(coder, LOG_TAKEN);
	return (true);
}
