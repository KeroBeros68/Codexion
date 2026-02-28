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

bool	compile(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	acquire_dongles(self);
	if (get_stop_sim(self->sim))
		return (false);
	self->compile_start = get_timestamp();
	set_deadline(self, get_timestamp() + self->sim->time_burnout);
	log_message(self, "is compiling");
	mysleep(self->sim->time_compile);
	release_dongle(self, self->right_dongle);
	release_dongle(self, self->left_dongle);
	return (true);
}

bool	debug(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	log_message(self, "is debugging");
	mysleep(self->sim->time_debug);
	return (true);
}

bool	refacto(t_coder *self)
{
	if (get_stop_sim(self->sim))
		return (false);
	log_message(self, "is refactoring");
	mysleep(self->sim->time_refactor);
	return (true);
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
