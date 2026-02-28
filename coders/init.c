/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:32:22 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:06:06 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** init_dongles
** Initializes all dongle entries in sim->tab_dongles.
** For each dongle, assigns a 1-based id, initializes its mutex and condition
** variable, and increments the corresponding inited counters so that clean()
** knows exactly how many resources must be destroyed on error.
**
** @param sim  Pointer to the simulation structure.
** @return     true on success, false if any mutex or cond init fails.
*/
bool	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->tab_dongles[i].waitlist.capacity = 2;
		if (pthread_mutex_init(&sim->tab_dongles[i].lock, NULL) != 0)
			return (cod_error(ERR_MUTEX_INIT));
		sim->inited.dongle_mutex++;
		if (pthread_cond_init(&sim->tab_dongles[i].cond, NULL) != 0)
			return (cod_error(ERR_COND_INIT));
		sim->inited.dongle_cond++;
		i++;
	}
	return (true);
}

/*
** init_coders
** Initializes all coder entries in sim->tab_coders.
** For each coder, assigns a 1-based id, binds left and right dongles in a
** circular layout (coder i holds dongle i and dongle i+1, with the last
** coder wrapping back to dongle 0), and stores a back-pointer to sim.
**
** @param sim  Pointer to the simulation structure.
*/
bool	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->tab_coders[i].id = i + 1;
		sim->tab_coders[i].deadline = UINT64_MAX;
		sim->tab_coders[i].left_dongle = &sim->tab_dongles[i];
		if (i == sim->nb_coders - 1)
			sim->tab_coders[i].right_dongle = &sim->tab_dongles[0];
		else
			sim->tab_coders[i].right_dongle = &sim->tab_dongles[i + 1];
		sim->tab_coders[i].sim = sim;
		if (pthread_mutex_init(&sim->tab_coders[i].cond_dead, NULL) != 0)
			return (cod_error(ERR_MUTEX_INIT));
		if (pthread_mutex_init(&sim->tab_coders[i].cond_nb_comp, NULL) != 0)
			return (cod_error(ERR_MUTEX_INIT));
		i++;
	}
	return (true);
}

/*
** init
** Allocates and initializes all simulation resources.
** Allocates coders and dongles tables with ft_calloc (zero-initialized),
** initializes the global log and sim mutexes, then delegates per-dongle
** and per-coder initialization to init_dongles() and init_coders().
** On any failure, returns false immediately; clean() uses the inited
** counters to destroy only what was successfully initialized.
**
** @param sim  Pointer to the simulation structure to initialize.
** @return     true on success, false on any allocation or init failure.
*/
bool	init(t_sim *sim)
{
	sim->tab_coders = ft_calloc(sim->nb_coders, sizeof(t_coder));
	if (!sim->tab_coders)
		return (cod_error(ERR_MALLOC_CODERS));
	sim->tab_dongles = ft_calloc(sim->nb_coders, sizeof(t_dongle));
	if (!sim->tab_dongles)
		return (cod_error(ERR_MALLOC_DONGLES));
	if (pthread_mutex_init(&sim->coder_finish_mutex, NULL) != 0)
		return (cod_error(ERR_MUTEX_INIT));
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (cod_error(ERR_MUTEX_INIT));
	sim->inited.log_mutex = true;
	if (pthread_mutex_init(&sim->sim_mutex, NULL) != 0)
		return (cod_error(ERR_MUTEX_INIT));
	sim->inited.sim_mutex = true;
	if (!init_dongles(sim))
		return (false);
	if (!init_coders(sim))
		return (false);
	return (true);
}
