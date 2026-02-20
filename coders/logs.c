/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:32:23 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 18:32:52 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <coders.h>

void	log_message(t_coder *coder, char *mes)
{
	struct timeval	tv;
	uint64_t		timestamp_ms;

	gettimeofday(&tv, NULL);
	timestamp_ms = (tv.tv_sec - coder->data->start_time.tv_sec) * 1000;
	timestamp_ms += (tv.tv_usec - coder->data->start_time.tv_usec) / 1000;

	pthread_mutex_lock(&coder->data->log_lock);
	printf("%llu %d %s\n", (unsigned long long)timestamp_ms, coder->id, mes);
	pthread_mutex_unlock(&coder->data->log_lock);
}
