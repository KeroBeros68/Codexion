/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 12:52:30 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 12:05:57 by kebertra         ###   ########.fr       */
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

static uint64_t	ft_atou64_s(const char *nptr, bool *error)
{
	int			i;
	uint64_t	ret;
	int			digit;

	i = 0;
	ret = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+')
		i++;
	else if (nptr[i] == '-')
	{
		*error = true;
		return (0);
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		digit = nptr[i] - '0';
		if (ret > (UINT64_MAX - digit) / 10)
		{
			*error = true;
			return (0);
		}
		ret = ret * 10 + digit;
		i++;
	}
	return (ret);
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
