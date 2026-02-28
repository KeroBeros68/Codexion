/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setget.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:59:58 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 22:59:26 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** get_stop_sim
** Thread-safe read of the simulation stop flag.
** Acquires sim_mutex before reading stop_sim to prevent data races
** with the thread that may set it concurrently.
**
** @param sim  Pointer to the simulation structure.
** @return     true if the simulation has been flagged to stop, false otherwise.
*/
bool	get_stop_sim(t_sim *sim)
{
	bool	value;

	pthread_mutex_lock(&sim->sim_mutex);
	value = sim->stop_sim;
	pthread_mutex_unlock(&sim->sim_mutex);
	if (value)
		return (true);
	return (false);
}

/*
** get_coders_finish
** Thread-safe read of the number of coders that have completed all their
** required compilations. Protected by coder_finish_mutex.
**
** @param sim  Pointer to the simulation structure.
** @return     Current value of coders_finish.
*/
int	get_coders_finish(t_sim *sim)
{
	int	value;

	pthread_mutex_lock(&sim->coder_finish_mutex);
	value = sim->coders_finish;
	pthread_mutex_unlock(&sim->coder_finish_mutex);
	return (value);
}

/*
** get_nb_compile
** Thread-safe read of the compilation counter for a single coder.
** Protected by cond_nb_comp.
**
** @param coder  Pointer to the coder.
** @return       Number of compilations completed so far.
*/
int	get_nb_compile(t_coder *coder)
{
	int	value;

	pthread_mutex_lock(&coder->cond_nb_comp);
	value = coder->nb_compile;
	pthread_mutex_unlock(&coder->cond_nb_comp);
	return (value);
}

/*
** get_deadline
** Thread-safe read of the burnout deadline for a single coder.
** Protected by cond_dead. The deadline equals compile_start +
** time_burnout, updated at the start of each compile cycle.
**
** @param coder  Pointer to the coder.
** @return       Deadline timestamp in milliseconds.
*/
uint64_t	get_deadline(t_coder *coder)
{
	uint64_t	value;

	pthread_mutex_lock(&coder->cond_dead);
	value = coder->deadline;
	pthread_mutex_unlock(&coder->cond_dead);
	return (value);
}
