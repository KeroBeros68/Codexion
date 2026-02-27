/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:35:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 19:45:19 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

static void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	log_message(coder, "has taken a dongle");
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_unlock(&dongle->lock);
	dongle->time_end_cooldown = get_timestamp() + coder->sim->time_cooldown;
}

bool	acquire_dongles(t_coder *coder)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		take_dongle(coder, coder->left_dongle);
		take_dongle(coder, coder->right_dongle);
	}
	else
	{
		take_dongle(coder, coder->right_dongle);
		take_dongle(coder, coder->left_dongle);
	}
	return (false);
}
