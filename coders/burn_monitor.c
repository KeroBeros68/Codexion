/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burn_monitor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 14:57:37 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/21 16:36:09 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	*monitor(void *arg)
{
	t_data			*data;
	int				i;
	struct timeval	tv;
	uint64_t		current_ms;
	uint64_t		start_ms;

	data = (t_data *)arg;
	start_ms = (((uint64_t)data->start_time.tv_sec * 1000)
			+ (data->start_time.tv_usec / 1000));
	while (1)
	{
		i = 0;
		gettimeofday(&tv, NULL);

		current_ms = ((uint64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);

		while (i < data->nb_coders)
		{
			if (current_ms - start_ms > (data->coder_list[i].last_compile
					+ data->time_burnout))
			{
				data->start_simulation = false;
				log_message(&data->coder_list[i], "burnout");
				return (NULL);
			}
			i++;
		}
		usleep(100);
	}
}
