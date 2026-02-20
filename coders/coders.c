/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 18:36:29 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

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
	t_data		data;
	pthread_t	*threads;

	memset(&data, 0, sizeof(t_data));
	if (ac < 9)
		return (1);
	if (!parser(av, &data))
		return (1);
	if (!init(&data))
		return (1);
	gettimeofday(&data.start_time, NULL);
	threads = malloc(sizeof(pthread_t) * data.nb_coders);
	if (!threads)
		return (clean(&data), cod_error(&data,
				"Main, failled malloc thread"), 1);
	if (!start_thread(&data, threads))
		return (1);
	stop_thread(&data, threads);
	return (0);
}
