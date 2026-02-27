/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:59:50 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 19:40:21 by kebertra         ###   ########.fr       */
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

# define ERR_USAGE			"Usage: ./codexion number_of_coders \
time_to_burnout time_to_compile time_to_debug \
time_to_refactor number_of_compiles_required \
dongle_cooldown scheduler(fifo|edf)\n"
# define ERR_NOT_POSITIVE_INT	"Argument is not a valid positive integer\n"
# define ERR_OVERFLOW			"Argument value causes integer overflow\n"
# define ERR_INVALID_SCHEDULER	"Scheduler must be exactly \"fifo\" or \
\"edf\"\n"
# define ERR_ZERO_CODERS		"number_of_coders must be greater than 0\n"
# define ERR_ZERO_COMPILES		"number_of_compiles_required must be greater \
than 0\n"
# define ERR_MALLOC_CODERS		"Memory allocation failed for coders table\n"
# define ERR_MALLOC_DONGLES		"Memory allocation failed for dongles table\n"
# define ERR_MUTEX_INIT			"Failed to initialize mutex\n"
# define ERR_COND_INIT			"Failed to initialize condition variable\n"

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
	t_heap_node	nodes[2];
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

/* **** Init tracking (true = successfully initialized, needs cleanup) */
typedef struct s_inited
{
	bool	log_mutex;
	bool	sim_mutex;
	int		dongle_mutex;
	int		dongle_cond;

}	t_inited;

/* **** Global Data structure */
typedef struct s_sim
{
	bool			stop_sim;
	t_inited		inited;

	pthread_t		monitor;

	pthread_mutex_t	log_mutex;
	pthread_mutex_t	sim_mutex;

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

/* ****	Mem *****/

void		ft_bzero(void *s, size_t n);
void		*ft_calloc(size_t nmemb, size_t size);

/* ****	Init ****/

bool		init(t_sim *sim);

/* ****	Clean ***/

void		clean(t_sim *sim);

/* ****	Parser **/

bool		parser(char **av, t_sim *sim);

/* ****	Log *****/

void		log_message(t_coder *coder, char *mes);

/* ****	Sim *****/

void		*monitoring(void *arg);
bool		stop_sim(t_sim *sim);
void		simulation(t_sim *sim);

bool		acquire_dongles(t_coder *coder);
void		release_dongle(t_coder *coder, t_dongle *dongle);

/* ****	Error ***/

bool		cod_error(t_sim *sim, char *msg);

#endif