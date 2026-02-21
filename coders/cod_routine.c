/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cod_routine.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:36:34 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/21 16:19:08 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	time_action(t_coder *coder, char *msg, long time)
{
	log_message(coder, msg);
	mysleep(time);
}

void	left_or_right_handed(t_coder *coder, pthread_mutex_t *first,
	pthread_mutex_t *second)
{
	if (coder->id % 2 == 0)
	{
		first = &coder->left_dongle->dongle_lock;
		second = &coder->right_dongle->dongle_lock;
	}
	else
	{
		first = &coder->right_dongle->dongle_lock;
		second = &coder->left_dongle->dongle_lock;
	}
}

void	take_dongle(t_coder *coder, pthread_mutex_t *first,
	pthread_mutex_t *second)
{
	(void)coder;
	(void)first;
	(void)second;
	return ;
}

bool	compile(void)
{
	return (true);
}

void	*coder_routine(void *arg)
{
	t_coder			*coder;
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;
	int				i;

	first = NULL;
	second = NULL;
	i = 0;
	coder = (t_coder *)arg;
	left_or_right_handed(coder, first, second);

	while (i < coder->data->nb_compile && coder->data->start_simulation)
	{

		if (!compile())
			break ;

		time_action(coder, "is debugging", coder->data->time_debug);
		time_action(coder, "is refactoring", coder->data->time_refactor);
		i++;
	}
	return (NULL);
}
