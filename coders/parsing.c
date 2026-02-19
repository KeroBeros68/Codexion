/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 12:52:30 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/19 12:53:40 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

static bool	schedule_validation(char *value, t_data *data)
{
	if (!strcmp(value, "FIFO") || !strcmp(value, "fifo"))
		data->scheduler = FIFO;
	else if (!strcmp(value, "EDF") || !strcmp(value, "edf"))
		data->scheduler = EDF;
	else
		return (false);
	return (true);
}

static bool	ft_str_check(char *s, int (*f)(int))
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

static int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

static bool	number_validation(void *data, char *s)
{
	long long	*ptr;

	ptr = (long long *)data;
	if (ft_str_check(s, ft_isdigit))
	{
		*ptr = atoi(s);
		return (true);
	}
	return (false);
}

bool	parser(char **av, t_data *data)
{
	if (!number_validation(&data->nb_coders, av[1])
		|| !number_validation(&data->time_burnout, av[2])
		|| !number_validation(&data->time_compile, av[3])
		|| !number_validation(&data->time_debug, av[4])
		|| !number_validation(&data->time_refactor, av[5])
		|| !number_validation(&data->nb_compile, av[6])
		|| !number_validation(&data->dongle_cooldown, av[7])
		|| !schedule_validation(av[8], data))
		return (false);
	return (true);
}