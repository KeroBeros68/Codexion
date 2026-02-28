/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:13:52 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:04:25 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** simulation
** Records the simulation start time and launches all threads.
** Starts the monitor thread first, then spawns one coder thread per coder.
** Returns immediately — threads run concurrently. clean() is responsible
** for joining all threads before resources are freed.
**
** @param sim  Pointer to the fully initialized simulation structure.
*/
void	simulation(t_sim *sim)
{
	int	i;

	i = 0;
	sim->start_time = get_timestamp();
	pthread_create(&sim->monitor, NULL, monitoring, sim);
	while (i < sim->nb_coders)
	{
		pthread_create(&sim->tab_coders[i].coder_thread, NULL, coder_routine,
			&sim->tab_coders[i]);
		i++;
	}
}
