/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/26 17:04:32 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

int	main(int ac, char **av)
{
	t_sim	sim;

	ft_bzero(&sim, sizeof(t_sim));
	if (ac != 9)
		return (cod_error(&sim, ERR_USAGE), 1);
	if (!parser(av, &sim))
		return (1);
	if (!init(&sim))
		return (clean(&sim), 1);
	clean(&sim);
	return (0);
}
