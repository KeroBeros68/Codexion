/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:13:52 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 22:07:52 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	compile(t_coder *self)
{
	self->compile_start = get_timestamp();
	set_deadline(self, get_timestamp() + self->sim->time_burnout);
	log_message(self, "is compiling");
	mysleep(self->sim->time_compile);

	release_dongle(self, self->right_dongle);
	release_dongle(self, self->left_dongle);
}

void	debug(t_coder *self)
{
	log_message(self, "is debugging");
	mysleep(self->sim->time_debug);
}

void	refacto(t_coder *self)
{
	log_message(self, "is refactoring");
	mysleep(self->sim->time_refactor);
}

void	*coder_routine(void *arg)
{
	t_coder	*self;
	int		nb_compile;

	self = (t_coder *)arg;
	set_deadline(self, get_timestamp() + self->sim->time_burnout);
	nb_compile = get_nb_compile(self);
	while (nb_compile < self->sim->total_compile)
	{
		if (get_stop_sim(self->sim))
			return (NULL);
		acquire_dongles(self);
		if (get_stop_sim(self->sim))
			return (NULL);
		compile(self);

		if (get_stop_sim(self->sim))
			return (NULL);
		debug(self);

		if (get_stop_sim(self->sim))
			return (NULL);
		refacto(self);
		set_nb_compile(self, ++nb_compile);
	}
	set_coders_finish(self->sim, 1);
	return (NULL);
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
}



