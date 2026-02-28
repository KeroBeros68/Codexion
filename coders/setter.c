/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 22:58:26 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 22:59:29 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** set_stop_sim
** Thread-safe write of the simulation stop flag.
** Acquires sim_mutex before writing, ensuring that no other thread
** reads a partially written value.
**
** @param sim    Pointer to the simulation structure.
** @param value  Value to assign (true = stop, false = running).
*/
void	set_stop_sim(t_sim *sim, bool value)
{
	pthread_mutex_lock(&sim->sim_mutex);
	sim->stop_sim = value;
	pthread_mutex_unlock(&sim->sim_mutex);
}

/*
** set_coders_finish
** Thread-safe increment of coders_finish by value.
** Called by each coder thread upon completing all required compilations.
** Protected by coder_finish_mutex.
**
** @param sim    Pointer to the simulation structure.
** @param value  Amount to add to coders_finish (always 1 in practice).
*/
void	set_coders_finish(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->coder_finish_mutex);
	sim->coders_finish += value;
	pthread_mutex_unlock(&sim->coder_finish_mutex);
}

/*
** set_nb_compile
** Thread-safe write of the compilation counter for a single coder.
** Protected by cond_nb_comp.
**
** @param coder  Pointer to the coder.
** @param value  New value for nb_compile.
*/
void	set_nb_compile(t_coder *coder, int value)
{
	pthread_mutex_lock(&coder->cond_nb_comp);
	coder->nb_compile = value;
	pthread_mutex_unlock(&coder->cond_nb_comp);
}

/*
** set_deadline
** Thread-safe write of the burnout deadline for a single coder.
** Called at the start of each compile cycle with compile_start +
** time_burnout. Protected by cond_dead.
**
** @param coder  Pointer to the coder.
** @param value  New deadline in milliseconds.
*/
void	set_deadline(t_coder *coder, uint64_t value)
{
	pthread_mutex_lock(&coder->cond_dead);
	coder->deadline = value;
	pthread_mutex_unlock(&coder->cond_dead);
}
