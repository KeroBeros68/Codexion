/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 14:04:42 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 17:16:22 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** get_timestamp
** Returns the current time as a Unix timestamp in milliseconds.
** Uses gettimeofday() for real-time precision.
**
** @return  Current time in milliseconds since the Unix epoch.
*/
uint64_t	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*
** mysleep
** Busy-waits for the specified duration in milliseconds.
** Polls get_timestamp() every 500 microseconds to avoid oversleeping
** while keeping CPU usage reasonable.
**
** @param time_in_ms  Duration to sleep in milliseconds.
*/
void	mysleep(long time_in_ms)
{
	uint64_t	start;
	uint64_t	now;

	start = get_timestamp();
	now = 0;
	while (now - start < (uint64_t)time_in_ms)
	{
		usleep(500);
		now = get_timestamp();
	}
}

/*
** ft_str_check
** Checks that every character in string s satisfies the predicate f.
** Returns false immediately if s or f is NULL, or if any character
** does not satisfy f.
**
** @param s  Null-terminated string to check.
** @param f  Character predicate (e.g. ft_isdigit).
** @return   true if all characters satisfy f, false otherwise.
*/
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

/*
** ft_isdigit
** Tests whether the character c is a decimal digit ('0'–'9').
**
** @param c  Character to test (as int).
** @return   Non-zero if c is a digit, 0 otherwise.
*/
int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

/*
** ft_atou64_s
** Converts the string nptr to an unsigned 64-bit integer.
** Skips leading whitespace, accepts an optional leading '+'.
** Sets *error to true and returns 0 on any of the following:
**   - a leading '-' sign (negative value)
**   - an overflow beyond UINT64_MAX
**
** @param nptr   Null-terminated string to convert.
** @param error  Output flag set to true on conversion error.
** @return       Parsed value, or 0 if *error was set.
*/
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
		return (*error = true, 0);
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		digit = nptr[i] - '0';
		if (ret > (UINT64_MAX - digit) / 10)
			return (*error = true, 0);
		ret = ret * 10 + digit;
		i++;
	}
	return (ret);
}
