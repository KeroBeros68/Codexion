/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/19 11:39:40 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"
#include <stdlib.h>


bool	schedule_validation(char *value, t_data *data)
{
	if (!strcmp(value, "FIFO") || !strcmp(value, "fifo"))
		data->scheduler = FIFO;
	else if (!strcmp(value, "EDF") || !strcmp(value, "edf"))
		data->scheduler = EDF;
	else
		return (false);
	return (true);
}

bool	ft_str_check(char *s, int (*f)(int))
{
	size_t	i;

	i = 0;
	if (!s || !f)
		return (false);
	while (s[i] != '\0')
	{
		if (!f(s[i]))
			return (false);
		i++;
	}
	return (true);
}

int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

bool	number_validation(int *data, char *s)
{
	if (ft_str_check(s, ft_isdigit)
		|| ((s[0] == '-' && ft_str_check(&s[1], ft_isdigit) && s[1])))
	{
		*data = atoi(s);
		return (true);
	}
	return (false);
}

bool	parser(char **av, t_data *data)
{
	if (!number_validation(&data->nb_coders, av[1]))
		printf("merde");
	data->time_burnout = atoi(av[2]);
	data->time_compile = atoi(av[3]);
	data->time_debug = atoi(av[4]);
	data->time_refactor = atoi(av[5]);
	data->nb_compile = atoi(av[6]);
	data->dongle_cooldown = atoi(av[7]);
	if (!schedule_validation(av[8], data))
		return (false);
	return (true);
}

int	main(int ac, char **av)
{
	int		i;
	t_data	data;

	i = 0;
	if (ac < 9)
		exit(1);
	if (!parser(av, &data))
{		printf("coucou");
		return (1);}
	printf("nb_coders: %d", data.nb_coders);
}
