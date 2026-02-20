/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cod_routine.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:36:34 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 20:44:08 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	*coder_routine(void *arg)
{
	t_coder			*coder;
	pthread_mutex_t	*left;
	pthread_mutex_t	*right;
	int				i;

	i = 0;
	coder = (t_coder *)arg;
	left = &coder->left_dongle->dongle_lock;
	right = &coder->right_dongle->dongle_lock;
	while (i < coder->data->nb_compile)
	{

		log_message(coder, "has taken a dongle");
		pthread_mutex_lock(left);
		log_message(coder, "has taken a dongle");
		pthread_mutex_lock(right);
		log_message(coder, "is compiling");
		mysleep(coder->data->time_compile);
		pthread_mutex_unlock(left);
		pthread_mutex_unlock(right);

		log_message(coder, "is debugging");
		mysleep(coder->data->time_debug);

		log_message(coder, "is refactoring");
		mysleep(coder->data->time_refactor);
		i++;
	}
	return (NULL);
}
