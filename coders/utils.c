/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 14:04:42 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 14:06:19 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"


uint64_t	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	mysleep(long time_in_ms)
{
	uint64_t	start;
	uint64_t	now;

	start = get_timestamp();
	now = 0;
	while (now - start < time_in_ms)
	{
		usleep(500);
		now = get_timestamp();
	}
}
