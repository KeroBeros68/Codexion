/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:35:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 22:43:58 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

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
	log_message(coder, "has taken a dongle");
	pthread_mutex_unlock(&dongle->lock);
}

uint64_t	scheduler(t_coder *coder)
{
	if (coder->sim->priority)
		return (coder->compile_start + coder->sim->time_burnout);
	return (get_timestamp());
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->time_end_cooldown = get_timestamp() + coder->sim->time_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

bool	acquire_dongles(t_coder *coder)
{
	uint64_t	key;

	key = scheduler(coder);
	take_dongle(coder, coder->left_dongle, key);
	take_dongle(coder, coder->right_dongle, key);
	return (false);
}
