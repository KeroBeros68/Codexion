/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:05:29 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:46:48 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

static bool	init_dongle(t_data *data)
{
	int	i;

	i = 0;

	data->dongle_list = malloc(sizeof(t_dongle) * data->nb_coders);
	if (!data->dongle_list)
		return (cod_error(data, "init_dongle, failled malloc dongle_list"));

	while (i < data->nb_coders)
	{
		data->dongle_list[i].id = i + 1;

		if (pthread_mutex_init(&data->dongle_list[i].dongle_lock, NULL) != 0)
			return (cod_error(data, "init_dongle, failled init mutex"));
		i++;
	}
	return (true);
}

static bool	init_coder(t_data *data)
{
	int	i;

	i = 0;

	data->coder_list = malloc(sizeof(t_coder) * data->nb_coders);
	if (!data->coder_list)
		return (cod_error(data, "init_coder, failled malloc coder_list"));

	while (i < data->nb_coders)
	{
		data->coder_list[i].id = i + 1;
		data->coder_list[i].left_dongle = &data->dongle_list[i];
		if (i + 1 == data->nb_coders)
			data->coder_list[i].right_dongle = &data->dongle_list[0];
		else
			data->coder_list[i].right_dongle = &data->dongle_list[i + 1];
		data->coder_list[i].data = data;
		i++;
	}
	return (true);
}

bool	init(t_data *data)
{
	if (pthread_mutex_init(&data->log_lock, NULL) != 0)
		return (cod_error(data, "init, failled init mutex log_lock"));
	if (pthread_mutex_init(&data->death_lock, NULL) != 0)
		return (cod_error(data, "init, failled init mutex death_lock"));

	if (!init_dongle(data))
		return (false);

	if (!init_coder(data))
		return (false);

	return (true);
}
