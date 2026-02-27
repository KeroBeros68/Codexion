/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:13:52 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 19:40:59 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"



void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	self->time_last_compile = get_timestamp();
	self->deadline = get_timestamp() + self->sim->time_burnout;
	while (self->nb_compile < self->sim->total_compile)
	{
		if (stop_sim(self->sim))
			return (NULL);
		acquire_dongles(self);
		if (stop_sim(self->sim))
			return (NULL);
		self->deadline = get_timestamp() + self->sim->time_burnout;
		log_message(self, "is compiling");
		mysleep(self->sim->time_compile);

		release_dongle(self, self->right_dongle);
		release_dongle(self, self->left_dongle);

		self->time_last_compile = get_timestamp();
		if (stop_sim(self->sim))
			return (NULL);
		log_message(self, "is debugging");
		mysleep(self->sim->time_debug);

		if (stop_sim(self->sim))
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



