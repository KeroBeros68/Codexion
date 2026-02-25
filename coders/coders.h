/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:59:50 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 14:07:40 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODERS_H
# define CODERS_H

/* ******************* INCLUDE ****************** */

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>
# include <limits.h>
# include <pthread.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <errno.h>

/* ******************* DEFINE ******************* */

/* ******************** ENUM ******************** */

/* ******************* STRUCT ******************* */

/* ****************** FUNCTION ****************** */

/* ****	Utils ***/

uint64_t	get_timestamp(void);
void		mysleep(long time_in_ms);

#endif