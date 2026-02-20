/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 12:52:30 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 13:14:17 by kebertra         ###   ########.fr       */
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
		return (cod_error(data,
				"schedule_validation, invalid data for scheduler"));
	return (true);
}

static bool	number_validation(void *data, char *s, size_t size)
{
	uint64_t	*ptr64;
	int			*ptr32;
	bool		error;

	error = false;
	if (!ft_str_check(s, ft_isdigit))
	{
		return (cod_error(data,
				"number_validation, not a positive integer"));
	}
	if (size == sizeof(uint64_t))
	{
		ptr64 = (uint64_t *)data;
		*ptr64 = ft_atou64_s(s, &error);
	}
	else if (size == sizeof(int))
	{
		ptr32 = (int *)data;
		*ptr32 = (int)ft_atou64_s(s, &error);
	}
	if (error)
		return (cod_error(data, "number_validation, overflow detected"));
	return (true);
}

bool	parser(char **av, t_data *data)
{
	if (!number_validation(&data->nb_coders, av[1], sizeof(int))
		|| !number_validation(&data->time_burnout, av[2], sizeof(uint64_t))
		|| !number_validation(&data->time_compile, av[3], sizeof(uint64_t))
		|| !number_validation(&data->time_debug, av[4], sizeof(uint64_t))
		|| !number_validation(&data->time_refactor, av[5], sizeof(uint64_t))
		|| !number_validation(&data->nb_compile, av[6], sizeof(int))
		|| !number_validation(&data->dongle_cooldown, av[7], sizeof(uint64_t))
		|| !schedule_validation(av[8], data))
		return (false);
	return (true);
}
