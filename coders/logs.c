/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:26:58 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/28 23:05:38 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** log_message
** Prints a timestamped simulation event to stdout in a thread-safe manner.
** Acquires log_mutex before writing to ensure that concurrent calls from
** multiple coder threads never produce interleaved output.
** Output format: "<timestamp_ms> <coder_id> <message>"
**
** @param coder  Pointer to the coder emitting the event.
** @param mes    Null-terminated string describing the event.
*/
void	log_message(t_coder *coder, char *mes)
{
	uint64_t	timestamp_ms;

	timestamp_ms = get_timestamp() - coder->sim->start_time;
	pthread_mutex_lock(&coder->sim->log_mutex);
	printf("%llu %d %s\n", (unsigned long long)timestamp_ms, coder->id, mes);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
