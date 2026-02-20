/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:19:48 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/20 11:40:48 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

bool	cod_error(t_data *data, char *msg)
{
	(void)data;
	fprintf(stderr, "ERROR: %s", msg);
	return (false);
}
