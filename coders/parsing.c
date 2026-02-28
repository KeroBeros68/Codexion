/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 12:52:30 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/25 16:53:19 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** schedule_validation
** Validates and stores the scheduler type from the given string.
** Accepts exactly "fifo" or "edf" (lowercase only, as per the subject).
** Writes the corresponding t_enum_sched value into sim->priority.
**
** @param value  Raw string from argv representing the scheduler type.
** @param sim    Pointer to the simulation structure to update.
** @return       true on success, false if the value is not recognized.
*/
static bool	schedule_validation(char *value, t_sim *sim)
{
	if (!strcmp(value, "fifo"))
		sim->priority = FIFO;
	else if (!strcmp(value, "edf"))
		sim->priority = EDF;
	else
		return (cod_error(ERR_INVALID_SCHEDULER));
	return (true);
}

/*
** number_validation
** Validates that string s represents a non-negative integer, then converts
** it and writes the result into dest. The size parameter selects the target
** type: sizeof(uint64_t) writes a uint64_t, sizeof(int) writes an int.
** Reports an error via cod_error() if s contains non-digit characters or
** if the converted value overflows the target type.
**
** @param dest  Pointer to the field to write (uint64_t * or int *).
** @param s     Raw string from argv to validate and convert.
** @param size  sizeof the target type, used to select the conversion path.
** @param sim   Pointer to the simulation structure, used for error reporting.
** @return      true on success, false on invalid input or overflow.
*/
static bool	number_validation(void *dest, char *s, size_t size)
{
	uint64_t	*dest_u64;
	int			*dest_int;
	bool		error;

	error = false;
	if (!ft_str_check(s, ft_isdigit))
	{
		return (cod_error(ERR_NOT_POSITIVE_INT));
	}
	if (size == sizeof(uint64_t))
	{
		dest_u64 = (uint64_t *)dest;
		*dest_u64 = ft_atou64_s(s, &error);
	}
	else if (size == sizeof(int))
	{
		dest_int = (int *)dest;
		*dest_int = (int)ft_atou64_s(s, &error);
	}
	if (error)
		return (cod_error(ERR_OVERFLOW));
	return (true);
}

/*
** parser
** Parses and validates all command-line arguments, then stores them in sim.
** Expected argument order (av[1] to av[8]):
**   nb_coders, time_to_burnout, time_to_compile, time_to_debug,
**   time_to_refactor, number_of_compiles_required,
**   dongle_cooldown, scheduler ("fifo" or "edf")
** Rejects negative numbers, non-integers, values <= 0 for nb_coders and
** total_compile, and any scheduler string other than "fifo" or "edf".
**
** @param av   Argument vector from main (must have at least 9 entries).
** @param sim  Pointer to the simulation structure to populate.
** @return     true if all arguments are valid, false otherwise.
*/
bool	parser(char **av, t_sim *sim)
{
	if (!number_validation(&sim->nb_coders, av[1], sizeof(int))
		|| !number_validation(&sim->time_burnout, av[2], sizeof(uint64_t))
		|| !number_validation(&sim->time_compile, av[3], sizeof(uint64_t))
		|| !number_validation(&sim->time_debug, av[4], sizeof(uint64_t))
		|| !number_validation(&sim->time_refactor, av[5], sizeof(uint64_t))
		|| !number_validation(&sim->total_compile, av[6], sizeof(int))
		|| !number_validation(&sim->time_cooldown, av[7], sizeof(uint64_t))
		|| !schedule_validation(av[8], sim))
		return (false);
	if (sim->nb_coders <= 0)
		return (cod_error(ERR_ZERO_CODERS));
	if (sim->total_compile <= 0)
		return (cod_error(ERR_ZERO_COMPILES));
	return (true);
}
