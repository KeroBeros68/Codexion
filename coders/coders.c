/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:18:59 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/19 12:52:22 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"
#include <stdlib.h>

int	main(int ac, char **av)
{
	int		i;
	t_data	data;

	i = 0;
	if (ac < 9)
		return (1);
	if (!parser(av, &data)) //si oui sortie en erreur
		return (1);
	printf("nb_coders: %d", data.nb_coders);
}
