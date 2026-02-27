/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 18:09:22 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 18:15:11 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	*monitoring(void *arg)
{
	t_sim			*sim;
	int				i;
	uint64_t		current_ms;

	sim = (t_sim *)arg;

	while (1)
	{
		usleep(100);
		pthread_mutex_lock(&sim->sim_mutex);
		if (sim->stop_sim)
		{
			pthread_mutex_unlock(&sim->sim_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&sim->sim_mutex);
		i = 0;
		current_ms = get_timestamp();
		while (i < sim->nb_coders)
		{
			if (current_ms > sim->tab_coders[i].deadline)
			{
				log_message(&sim->tab_coders[i], "burned out");
				pthread_mutex_lock(&sim->sim_mutex);
				sim->stop_sim = true;
				pthread_mutex_unlock(&sim->sim_mutex);
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}
