/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 10:41:38 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"
#include <stdlib.h>

bool	init(t_data *data)
{
	int			i;

	i = 0;
	data->dongle_list = malloc(sizeof(t_dongle) * data->nb_coders);
	if (!data->dongle_list)
		return (false);
	while (i < data->nb_coders)
	{
		data->dongle_list[i].id = i + 1;

		if (pthread_mutex_init(&data->dongle_list[i].dongle_lock, NULL) != 0)
			return (false);
		i++;
	}
	data->coder_list = malloc(sizeof(t_coder) * data->nb_coders);
	if (!data->coder_list)
		return (false);
	if (pthread_mutex_init(&data->log_lock, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&data->death_lock, NULL) != 0)
		return (false);
	i = 0;
	while (i < data->nb_coders)
	{
		data->coder_list[i].id = i + 1;
		data->coder_list[i].left_dongle = &data->dongle_list[i];
		if (i + 1 == data->nb_coders)
			data->coder_list[i].right_dongle = &data->dongle_list[0];
		else
			data->coder_list[i].right_dongle = &data->dongle_list[i + 1];
		i++;
	}
	return (true);
}

bool	clean(t_data *data)
{
	int	i;

	if (!data)
		return (false);
	if (data->dongle_list)
	{
		i = 0;
		// On ne détruit que ce qui a été alloué/init si possible
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

void	print_coders_info(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		printf("Coders ID: %d\n", data->coder_list[i].id);
		printf("Left dongle ID: %d\n", data->coder_list[i].left_dongle->id);
		printf("Right dongle ID: %d\n\n", data->coder_list[i].right_dongle->id);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (ac < 9)
		return (1);
	if (!parser(av, &data)) //si oui sortie en erreur, penser a safe le atoi
		return (1);
	if (!init(&data))
		return (1);
	print_coders_info(&data);
	clean(&data);
	return (0);
}
