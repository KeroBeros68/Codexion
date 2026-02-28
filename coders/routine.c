/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:00:36 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:05:07 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** compile
** Runs one compilation cycle for a coder.
** Checks stop_sim, acquires both dongles, records compile_start, updates
** deadline, logs "is compiling", sleeps time_compile, releases both dongles.
** Returns false immediately if stop_sim is set before or after acquisition.
**
** @param self  Pointer to the coder executing the compile phase.
** @return      true on success, false if stop_sim was set.
*/
bool	compile(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	acquire_dongles(self);
	if (get_stop_sim(self->sim))
		return (false);
	self->compile_start = get_timestamp();
	set_deadline(self, get_timestamp() + self->sim->time_burnout);
	log_message(self, LOG_COMPILING);
	mysleep(self->sim->time_compile);
	release_dongle(self, self->right_dongle);
	release_dongle(self, self->left_dongle);
	return (true);
}

/*
** debug
** Runs the debug phase for a coder.
** Checks stop_sim, logs "is debugging", sleeps for time_debug.
**
** @param self  Pointer to the coder executing the debug phase.
** @return      true on success, false if stop_sim was set.
*/
bool	debug(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	log_message(self, LOG_DEBUGGING);
	mysleep(self->sim->time_debug);
	return (true);
}

/*
** refacto
** Runs the refactoring phase for a coder.
** Checks stop_sim, logs "is refactoring", sleeps for time_refactor.
**
** @param self  Pointer to the coder executing the refacto phase.
** @return      true on success, false if stop_sim was set.
*/
bool	refacto(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	log_message(self, LOG_REFACTORING);
	mysleep(self->sim->time_refactor);
	return (true);
}

/*
** coder_routine
** Entry point for each coder thread.
** Sets initial deadline, then loops compile -> debug -> refacto until
** total_compile is reached or stop_sim is set. Each sub-phase returns
** false if stopped mid-cycle, causing immediate thread exit.
** On normal completion increments coders_finish and returns NULL.
**
** @param arg  Pointer to the coder's t_coder struct, cast as void *.
** @return     Always NULL.
*/
void	*coder_routine(void *arg)
{
	t_coder	*self;
	int		nb_compile;

	self = (t_coder *)arg;
	set_deadline(self, get_timestamp() + self->sim->time_burnout);
	nb_compile = get_nb_compile(self);
	while (nb_compile < self->sim->total_compile)
	{
		if (!compile(self))
			return (NULL);
		if (!debug(self))
			return (NULL);
		if (!refacto(self))
			return (NULL);
		set_nb_compile(self, ++nb_compile);
	}
	set_coders_finish(self->sim, 1);
	return (NULL);
}
