/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:14:40 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:15:37 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

bool	clean(t_data *data)
{
	int	i;

	if (!data)
		return (false);
	if (data->dongle_list)
	{
		i = 0;
		while (i < data->nb_coders)
		{
			pthread_mutex_destroy(&data->dongle_list[i].dongle_lock);
			i++;
		}
		free(data->dongle_list);
	}
	if (data->coder_list)
		free(data->coder_list);
	pthread_mutex_destroy(&data->log_lock);
	pthread_mutex_destroy(&data->death_lock);
	return (true);
}
