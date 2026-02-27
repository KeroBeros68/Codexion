/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:13:52 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 18:09:45 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	log_message(coder, "has taken a dongle");
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_unlock(&dongle->lock);
	dongle->time_end_cooldown = get_timestamp() + coder->sim->time_cooldown;
}

bool	sim_is_stopped(t_sim *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->sim_mutex);
	stopped = sim->stop_sim;
	pthread_mutex_unlock(&sim->sim_mutex);
	return (stopped);
}

void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	self->time_last_compile = get_timestamp();
	self->deadline = get_timestamp() + self->sim->time_burnout;
	while (self->nb_compile < self->sim->total_compile)
	{
		if (sim_is_stopped(self->sim))
			return (NULL);
		take_dongle(self, self->right_dongle);
		if (sim_is_stopped(self->sim))
			return (NULL);
		take_dongle(self, self->left_dongle);
		if (sim_is_stopped(self->sim))
			return (NULL);
		self->deadline = get_timestamp() + self->sim->time_burnout;
		log_message(self, "is compiling");
		mysleep(self->sim->time_compile);

		release_dongle(self, self->right_dongle);
		release_dongle(self, self->left_dongle);

		self->time_last_compile = get_timestamp();
		if (sim_is_stopped(self->sim))
			return (NULL);
		log_message(self, "is debugging");
		mysleep(self->sim->time_debug);

		if (sim_is_stopped(self->sim))
			return (NULL);
		log_message(self, "is refactoring");
		mysleep(self->sim->time_refactor);
		self->nb_compile++;
	}
	return (NULL);
}

void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->tab_coders[i].coder_thread, NULL);
		i++;
	}
}



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
	join_coders(sim);
	pthread_mutex_lock(&sim->sim_mutex);
	sim->stop_sim = true;
	pthread_mutex_unlock(&sim->sim_mutex);
	pthread_join(sim->monitor, NULL);
}



