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
** Acquires sim_mutex before setting stop_sim to true, ensuring that
** no other thread reads a partially written value.
**
** @param sim  Pointer to the simulation structure.
*/
void	set_stop_sim(t_sim *sim, bool value)
{
	pthread_mutex_lock(&sim->sim_mutex);
	sim->stop_sim = value;
	pthread_mutex_unlock(&sim->sim_mutex);
}

void	set_coders_finish(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->coder_finish_mutex);
	sim->coders_finish += value;
	pthread_mutex_unlock(&sim->coder_finish_mutex);
}

void	set_nb_compile(t_coder *coder, int value)
{
	pthread_mutex_lock(&coder->cond_nb_comp);
	coder->nb_compile = value;
	pthread_mutex_unlock(&coder->cond_nb_comp);
}

void	set_deadline(t_coder *coder, uint64_t value)
{
	pthread_mutex_lock(&coder->cond_dead);
	coder->deadline = value;
	pthread_mutex_unlock(&coder->cond_dead);
}
