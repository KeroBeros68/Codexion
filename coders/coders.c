/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:55:53 by kebertra         ###   ########.fr       */
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

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	printf("coucou - avant sleep avec time_burnout: %lu\n", coder->data->time_burnout);
	usleep(coder->data->time_burnout);
	printf("je suis mort\n");
	return (NULL);
}

int	main(int ac, char **av)
{
	t_data	data;
	pthread_t thread;

	memset(&data, 0, sizeof(t_data));
	if (ac < 9)
		return (1);
	if (!parser(av, &data)) //si oui sortie en erreur, penser a safe le atoi
		return (1);
	if (!init(&data))
		return (1);
	//print_coders_info(&data);
	pthread_create(&thread, NULL, coder_routine, &data.coder_list[1]);
	pthread_join(thread, NULL);
	clean(&data);
	return (0);
}
