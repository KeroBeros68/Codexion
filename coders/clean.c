/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 16:13:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/26 17:07:09 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	clean(t_sim *sim)
{
	int	i;

	if (sim->tab_coders)
		free(sim->tab_coders);

	i = 0;
	while (i < sim->inited.dongle_cond)
	{
		pthread_cond_destroy(&sim->tab_dongles[i].cond);
		i++;
	}
	i = 0;
	while (i < sim->inited.dongle_mutex)
	{
		pthread_mutex_destroy(&sim->tab_dongles[i].lock);
		i++;
	}
	if (sim->tab_dongles)
		free(sim->tab_dongles);

	if (sim->inited.log_mutex)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->inited.sim_mutex)
		pthread_mutex_destroy(&sim->sim_mutex);
}
