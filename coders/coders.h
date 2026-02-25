/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:59:50 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 17:16:53 by kebertra         ###   ########.fr       */
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

/* **** Enumeration for Scheduler mode */
typedef enum e_scheduler_type
{
	FIFO = 0,
	EDF = 1
}	t_enum_sched;

/* ******************* STRUCT ******************* */

typedef struct s_sim	t_sim;
typedef struct s_dongle	t_dongle;

/* **** Heap node definition */
typedef struct s_heap_node
{
	int			coder_id;
	uint64_t	key;
}	t_heap_node;

/* **** Heap definition ******/
typedef struct s_heap
{
	t_heap_node	*nodes;
	int			size;
	int			capacity;
}	t_heap;

/* **** Dongle definition *****/
typedef struct s_dongle
{
	int				id;

	pthread_mutex_t	lock;
	pthread_cond_t	cond;

	uint64_t		time_end_cooldown;
	t_heap			waitlist;
}	t_dongle;

/* **** Coder definition ******/
typedef struct s_coder
{
	int			id;
	int			nb_compile;

	pthread_t	coder_thread;

	uint64_t	time_last_compile;
	uint64_t	deadline;

	t_dongle	*left_dongle;
	t_dongle	*right_dongle;

	t_sim		*sim;
}	t_coder;

/* **** Global Data structure */
typedef struct s_sim
{
	bool			stop_sim;

	pthread_t		monitor;

	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_sim_mutex;

	t_coder			*tab_coders;
	t_dongle		*tab_dongles;

	uint64_t		start_time;

	int				nb_coders;
	int				total_compile;
	uint64_t		time_burnout;
	uint64_t		time_compile;
	uint64_t		time_refactor;
	uint64_t		time_debug;
	uint64_t		time_cooldown;
	t_enum_sched	priority;

}	t_sim;

/* ****************** FUNCTION ****************** */

/* ****	Utils ***/

bool		ft_str_check(char *s, int (*f)(int));
int			ft_isdigit(int c);
uint64_t	ft_atou64_s(const char *nptr, bool *error);
uint64_t	get_timestamp(void);
void		mysleep(long time_in_ms);

/* ****	Parser **/

bool		parser(char **av, t_sim *sim);

/* ****	Error ***/

bool		cod_error(t_sim *sim, char *msg);

#endif