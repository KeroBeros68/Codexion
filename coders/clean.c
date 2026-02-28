/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 16:13:12 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:06:20 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->tab_coders[i].coder_thread, NULL);
		pthread_mutex_destroy(&sim->tab_coders[i].cond_dead);
		pthread_mutex_destroy(&sim->tab_coders[i].cond_nb_comp);
		i++;
	}
}

void	clean(t_sim *sim)
{
	int	i;

	pthread_join(sim->monitor, NULL);
	join_coders(sim);
	if (sim->tab_coders)
		free(sim->tab_coders);
	i = 0;
	while (i < sim->inited.dongle_cond)
		pthread_cond_destroy(&sim->tab_dongles[i++].cond);
	i = 0;
	while (i < sim->inited.dongle_mutex)
		pthread_mutex_destroy(&sim->tab_dongles[i++].lock);
	if (sim->tab_dongles)
		free(sim->tab_dongles);
	if (sim->inited.log_mutex)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->inited.sim_mutex)
		pthread_mutex_destroy(&sim->sim_mutex);
	pthread_mutex_destroy(&sim->coder_finish_mutex);
}
