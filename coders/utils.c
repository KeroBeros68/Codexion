/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:12:53 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 13:14:47 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

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

uint64_t	ft_atou64_s(const char *nptr, bool *error)
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
