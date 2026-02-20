/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:14:33 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"
#include <stdlib.h>

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

void	coder_routine(void)
{
	printf("coucou");
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
