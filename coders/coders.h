/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:19:14 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/21 16:24:37 by kebertra         ###   ########.fr       */
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

/* ******************* DEFINE ******************* */


/* ******************** ENUM ******************** */

/* **** Enumeration for Scheduler mode */
enum	e_scheduler_type
{
	FIFO = 0,
	EDF = 1
};


/* ******************* Struct ******************* */

typedef struct s_data	t_data;

/* **** Dongle definition */
typedef struct s_dongle
{
	int				id;

	pthread_mutex_t	dongle_lock;

	uint64_t		cooldown_end;
}	t_dongle;

/* **** Coder definition */
typedef struct s_coder
{
	int			id;

	uint64_t	last_compile;

	t_dongle	*left_dongle;
	t_dongle	*right_dongle;

	t_data		*data;
}	t_coder;

/* **** Global Data structure */
typedef struct s_data
{
	int						nb_coders;
	int						nb_compile;

	uint64_t				time_burnout;
	uint64_t				time_compile;
	uint64_t				time_debug;
	uint64_t				time_refactor;
	uint64_t				dongle_cooldown;

	enum e_scheduler_type	scheduler;

	pthread_mutex_t			log_lock;
	pthread_mutex_t			death_lock;

	struct timeval			start_time;
	bool					start_simulation;

	t_dongle				*dongle_list;
	t_coder					*coder_list;
}	t_data;


/* ****************** FUNCTION ****************** */

/* ****	Utils ***/

uint64_t	ft_atou64_s(const char *nptr, bool *error);
void		mysleep(long time_in_ms);
bool		ft_str_check(char *s, int (*f)(int));
int			ft_isdigit(int c);


/* ****	Parsing */

bool		parser(char **av, t_data *data);

/* ****	Init ****/

bool		init(t_data *data);

/* **** Clean ***/

bool		clean(t_data *data);

/* **** Error ***/

bool		cod_error(t_data *data, char *msg);

/* ****	Threads */

bool		start_thread(t_data *data, pthread_t *threads, pthread_t *monitors);
void		stop_thread(t_data *data, pthread_t *threads, pthread_t *monitors);

/* **** Routine */

void		*monitor(void *arg);
void		*coder_routine(void *arg);

/* **** Log *****/

void		log_message(t_coder *coder, char *mes);

#endif