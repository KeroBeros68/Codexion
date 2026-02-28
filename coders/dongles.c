/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:35:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 22:06:18 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

static void	take_dongle(t_coder *coder, t_dongle *dongle, uint64_t key)
{
	t_heap_node	node;

	node.coder_id = coder->id;
	node.key = key;
	pthread_mutex_lock(&dongle->lock);
	heap_push(&dongle->waitlist, node);
	while (heap_peek(&dongle->waitlist).coder_id != coder->id)
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	heap_pop(&dongle->waitlist);
	log_message(coder, "has taken a dongle");
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	dongle->time_end_cooldown = get_timestamp() + coder->sim->time_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

bool	acquire_dongles(t_coder *coder)
{
	uint64_t	key;

	key = get_timestamp();
	take_dongle(coder, coder->left_dongle, key);
	take_dongle(coder, coder->right_dongle, key);
	return (false);
}
