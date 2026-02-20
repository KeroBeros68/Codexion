/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:19:14 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 20:43:45 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODERS_H
# define CODERS_H

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>
# include <limits.h>
# include <pthread.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>

enum	e_scheduler_type
{
	FIFO = 0,
	EDF = 1
};

typedef struct s_data	t_data;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	dongle_lock;
}	t_dongle;

typedef struct s_coder
{
	int			id;

	t_dongle	*left_dongle;
	t_dongle	*right_dongle;

	t_data		*data;
}	t_coder;

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

	t_dongle				*dongle_list;
	t_coder					*coder_list;
}	t_data;

bool		parser(char **av, t_data *data);
bool		init(t_data *data);
bool		clean(t_data *data);
bool		cod_error(t_data *data, char *msg);

uint64_t	ft_atou64_s(const char *nptr, bool *error);
int			ft_isdigit(int c);
bool		ft_str_check(char *s, int (*f)(int));

bool		start_thread(t_data *data, pthread_t *threads);
void		stop_thread(t_data *data, pthread_t *threads);

void		*coder_routine(void *arg);

void		log_message(t_coder *coder, char *mes);

void		mysleep(long time_in_ms);
#endif