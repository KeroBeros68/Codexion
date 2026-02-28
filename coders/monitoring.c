/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 18:09:22 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:31:03 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** check_coders
** Iterates over all coders and checks whether any of them has exceeded
** its deadline without having completed all required compilations.
** If a burnout is detected, logs the event and returns true immediately.
** Coders that have already reached total_compile are skipped, as their
** deadline is no longer meaningful.
**
** @param sim         Pointer to the simulation structure.
** @param current_ms  Current timestamp in milliseconds.
** @return            true if at least one coder burned out, false otherwise.
*/
static bool	check_coders(t_sim *sim, uint64_t current_ms)
{
	int			i;
	int			nb_compile;
	int			finished;
	uint64_t	deadline;

	i = 0;
	while (i < sim->nb_coders)
	{
		nb_compile = get_nb_compile(&sim->tab_coders[i]);
		deadline = get_deadline(&sim->tab_coders[i]);
		if (current_ms > deadline && nb_compile != sim->total_compile)
		{
			log_message(&sim->tab_coders[i], LOG_BURNED_OUT);
			return (true);
		}
		i++;
	}
	finished = get_coders_finish(sim);
	if (sim->nb_coders == finished)
		return (true);
	return (false);
}

/*
** monitoring
** Entry point for the monitor thread. Runs concurrently with all coder
** threads and polls the simulation state every 100 microseconds.
** On each iteration it checks whether the stop flag has already been
** raised (e.g. by simulation() after a normal finish), then scans all
** coders for a burnout condition via check_coders().
** If a burnout is detected, the stop flag is set and the thread exits.
** The 100 µs poll granularity ensures the 10 ms detection constraint
** is always satisfied.
**
** @param arg  Pointer to the t_sim structure cast as void *.
** @return     Always NULL.
*/
void	*monitoring(void *arg)
{
	t_sim			*sim;
	uint64_t		current_ms;

	sim = (t_sim *)arg;
	while (1)
	{
		usleep(100);
		if (get_stop_sim(sim))
			return (NULL);
		current_ms = get_timestamp();
		if (check_coders(sim, current_ms))
		{
			set_stop_sim(sim, true);
			wake_all_dongles(sim);
			return (NULL);
		}
	}
	return (NULL);
}
