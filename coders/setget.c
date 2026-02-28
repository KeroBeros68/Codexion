/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setget.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:59:58 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 21:34:41 by kebertra         ###   ########.fr       */
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

/*
** stop_sim
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


void	set_coders_finish(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->coder_finish_mutex);
	sim->coders_finish += value;
	pthread_mutex_unlock(&sim->coder_finish_mutex);
}

int	get_coders_finish(t_sim *sim)
{
	int	value;

	pthread_mutex_lock(&sim->coder_finish_mutex);
	value = sim->coders_finish;
	pthread_mutex_unlock(&sim->coder_finish_mutex);
	return (value);
}

void	set_nb_compile(t_coder *coder, int value)
{
	pthread_mutex_lock(&coder->cond_nb_comp);
	coder->nb_compile = value;
	pthread_mutex_unlock(&coder->cond_nb_comp);
}

int	get_nb_compile(t_coder *coder)
{
	int	value;

	pthread_mutex_lock(&coder->cond_nb_comp);
	value = coder->nb_compile;
	pthread_mutex_unlock(&coder->cond_nb_comp);
	return (value);
}

void	set_deadline(t_coder *coder, uint64_t value)
{
	pthread_mutex_lock(&coder->cond_dead);
	coder->deadline = value;
	pthread_mutex_unlock(&coder->cond_dead);
}

uint64_t	get_deadline(t_coder *coder)
{
	uint64_t	value;

	pthread_mutex_lock(&coder->cond_dead);
	value = coder->deadline;
	pthread_mutex_unlock(&coder->cond_dead);
	return (value);
}
