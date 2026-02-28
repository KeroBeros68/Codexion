/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:19:48 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 16:53:55 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** cod_error
** Prints an error message to stderr and returns false.
** Designed for use in return statements: return (cod_error(MSG)).
**
** @param msg  Null-terminated error string (prefixed with "ERROR: ").
** @return     Always false.
*/
bool	cod_error(char *msg)
{
	fprintf(stderr, "ERROR: %s", msg);
	return (false);
}
