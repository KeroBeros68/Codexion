/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:32:22 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 17:43:08 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

bool	init(t_sim *sim)
{
	sim->tab_coders = malloc(sim->nb_coders * sizeof(t_coder));
	if (!sim->tab_coders)
		return (cod_error(sim, ERR_MALLOC_CODERS));
	sim->tab_dongles = malloc(sim->nb_coders * sizeof(t_dongle));
	if (!sim->tab_dongles)
		return (cod_error(sim, ERR_MALLOC_DONGLES));

	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->stop_sim_mutex, NULL);

	return (true);
}
