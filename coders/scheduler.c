/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 14:29:03 by kebertra          #+#    #+#             */
/*   Updated: 2026/03/01 14:29:19 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** scheduler
** Returns the sort key used to order coder requests in the dongle waitlist.
** FIFO (priority == 0): key = current timestamp at request time.
** EDF  (priority == 1): key = compile_start + time_burnout (earliest deadline).
** The same key is used for both left and right dongle acquisitions in a
** single compile cycle to prevent priority inversion between the two takes.
**
** @param coder  Pointer to the requesting coder.
** @return       Sort key as uint64_t milliseconds.
*/
uint64_t	scheduler(t_coder *coder)
{
	if (coder->sim->priority)
		return (coder->compile_start + coder->sim->time_burnout);
	return (get_timestamp());
}
