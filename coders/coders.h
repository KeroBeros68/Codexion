/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:19:14 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:25:41 by kebertra         ###   ########.fr       */
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

enum	e_scheduler_type
{
	FIFO = 0,
	EDF = 1
};

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

	t_dongle				*dongle_list;
	t_coder					*coder_list;
}	t_data;

bool	parser(char **av, t_data *data);
bool	init(t_data *data);
bool	clean(t_data *data);
bool	cod_error(t_data *data, char *msg);

#endif