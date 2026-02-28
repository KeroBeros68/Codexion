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

/*
** join_coders
** Joins every coder thread then destroys its per-coder mutexes.
** pthread_join is called before destroy to ensure the thread has fully
** exited and will no longer access cond_dead or cond_nb_comp.
**
** @param sim  Pointer to the simulation structure.
*/
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

/*
** clean
** Joins all threads and releases all simulation resources in safe order.
** Order: join monitor -> join_coders (join + destroy per-coder mutexes) ->
** free coders -> destroy dongle conds -> destroy dongle mutexes -> free
** dongles -> destroy global mutexes guarded by inited flags (log_mutex,
** sim_mutex, coder_finish_mutex). Monitor is joined first so it cannot
** access coder mutexes after they are destroyed.
**
** @param sim  Pointer to the simulation structure to clean up.
*/
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
	if (sim->inited.coder_finish_mutex)
		pthread_mutex_destroy(&sim->coder_finish_mutex);
}
