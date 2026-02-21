/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:57:38 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/21 16:25:31 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

bool	start_thread(t_data *data, pthread_t *threads, pthread_t *monitors)
{
	int	i;

	i = 0;
	if (pthread_create(monitors, NULL, monitor, data) != 0)
	{
		free(threads);
		clean(data);
		return (cod_error(data, "start_thread, create monitor threads"));
	}
	while (i < data->nb_coders)
	{
		if (pthread_create(&threads[i], NULL, coder_routine,
				&data->coder_list[i]) != 0)
		{
			free(threads);
			clean(data);
			return (cod_error(data, "start_thread, create coder threads"));
		}
		i++;
	}
	return (true);
}

void	stop_thread(t_data *data, pthread_t *threads, pthread_t *monitors)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(*monitors, NULL);
	free(threads);
	clean(data);
}
