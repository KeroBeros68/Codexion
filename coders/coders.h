/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:59:50 by kebertra          #+#    #+#             */
/*   Updated: 2026/03/01 14:30:13 by kebertra         ###   ########.fr       */
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

/* **** Colored log messages for simulation events */
# define LOG_TAKEN				"\033[1;34mhas taken a dongle\033[0m"
# define LOG_COMPILING			"\033[1;32mis compiling\033[0m"
# define LOG_DEBUGGING			"\033[1;33mis debugging\033[0m"
# define LOG_REFACTORING		"\033[1;33mis refactoring\033[0m"
# define LOG_BURNED_OUT			"\033[1;31mburned out\033[0m"

/* **** End-of-simulation ASCII art banner */
# define END_BANNER \
"\n  /$$$$$$  /$$                         /$$             /$$     /$$" \
"                           /$$$$$$$$                 /$$\n" \
" /$$__  $$|__/                        | $$            | $$    |__/" \
"                          | $$_____/                | $$\n" \
"| $$  \\__/ /$$ /$$$$$$/$$$$  /$$   /$$| $$  /$$$$$$  /$$$$$$   /$$" \
"  /$$$$$$  /$$$$$$$       | $$       /$$$$$$$   /$$$$$$$\n" \
"|  $$$$$$ | $$| $$_  $$_  $$| $$  | $$| $$ |____  $$|_  $$_/  | $$" \
" /$$__  $$| $$__  $$      | $$$$$   | $$__  $$ /$$__  $$\n" \
" \\____  $$| $$| $$ \\ $$ \\ $$| $$  | $$| $$  /$$$$$$$  | $$    | $$" \
"| $$  \\ $$| $$  \\ $$      | $$__/   | $$  \\ $$| $$  | $$\n" \
" /$$  \\ $$| $$| $$ | $$ | $$| $$  | $$| $$ /$$__  $$  | $$ /$$| $$" \
"| $$  | $$| $$  | $$      | $$      | $$  | $$| $$  | $$\n" \
"|  $$$$$$/| $$| $$ | $$ | $$|  $$$$$$/| $$|  $$$$$$$  |  $$$$/| $$" \
"|  $$$$$$/| $$  | $$      | $$$$$$$$| $$  | $$|  $$$$$$$\n" \
" \\______/ |__/|__/ |__/ |__/ \\______/ |__/ \\_______/   \\___/  |__/" \
" \\______/ |__/  |__/      |________/|__/  |__/ \\_______/\n\n"

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
	pthread_mutex_t	lock;
	pthread_cond_t	cond;

	bool			in_use;
	uint64_t		time_end_cooldown;
	t_heap			waitlist;
}	t_dongle;

/* **** Coder definition ******/
typedef struct s_coder
{
	int				id;
	int				nb_compile;

	pthread_t		coder_thread;
	pthread_mutex_t	cond_dead;
	pthread_mutex_t	cond_nb_comp;

	uint64_t		compile_start;
	uint64_t		deadline;

	t_dongle		*left_dongle;
	t_dongle		*right_dongle;

	t_sim			*sim;
}	t_coder;

/* **** Init tracking (true = successfully initialized, needs cleanup) */
typedef struct s_inited
{
	bool	log_mutex;
	bool	sim_mutex;
	bool	coder_finish_mutex;
	int		dongle_mutex;
	int		dongle_cond;

}	t_inited;

/* **** Global Data structure */
typedef struct s_sim
{
	bool			stop_sim;
	int				coders_finish;
	t_inited		inited;

	pthread_t		monitor;

	pthread_mutex_t	log_mutex;
	pthread_mutex_t	sim_mutex;
	pthread_mutex_t	coder_finish_mutex;

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

/* **** SetGet **/

bool		get_stop_sim(t_sim *sim);
void		set_stop_sim(t_sim *sim, bool value);
int			get_coders_finish(t_sim *sim);
void		set_coders_finish(t_sim *sim, int value);
int			get_nb_compile(t_coder *coder);
void		set_nb_compile(t_coder *coder, int value);
uint64_t	get_deadline(t_coder *coder);
void		set_deadline(t_coder *coder, uint64_t value);

/* ****	Sim *****/

void		*coder_routine(void *arg);
void		*monitoring(void *arg);
void		simulation(t_sim *sim);

/* ****	Dongle **/

bool		acquire_dongles(t_coder *coder);
void		release_dongle(t_coder *coder, t_dongle *dongle);
void		wake_all_dongles(t_sim *sim);

/* ****	Heap ****/

void		heap_push(t_heap *heap, t_heap_node node);
t_heap_node	heap_pop(t_heap *heap);
t_heap_node	heap_peek(t_heap *heap);

/* ****	Sched ***/

uint64_t	scheduler(t_coder *coder);

/* ****	Error ***/

bool		cod_error(char *msg);

#endif