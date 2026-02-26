/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 16:31:11 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/26 16:34:25 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/**
 * @brief Fills a memory block with a given byte.
 *
 * Writes `n` bytes of value `c` to the memory block pointed to by `s`.
 *
 * @param s  Pointer to the memory block to fill.
 * @param c  Byte to write (converted to unsigned char).
 * @param n  Number of bytes to fill.
 *
 * @return void*  Pointer to the memory block `s`.
 */
static void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*s2;

	i = 0;
	s2 = (unsigned char *) s;
	while (i < n)
	{
		s2[i] = (unsigned char)c;
		i++;
	}
	return (s);
}

/**
 * @brief Fills a memory block with zeros.
 *
 * Writes `n` bytes of zero value to the memory area pointed to by `s`.
 *
 * @param s  Pointer to the memory block to fill.
 * @param n  Number of bytes to set to zero.
 */
void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, 0, n);
}

/**
 * @brief Allocates memory and initializes all bytes to zero.
 *
 * Allocates a memory block of `nmemb * size` bytes
 * and fills it with zeros.
 * Checks for overflow when calculating the total size.
 *
 * @param nmemb  Number of elements to allocate.
 * @param size   Size of each element in bytes.
 *
 * @return void*  Pointer to the allocated memory block, or NULL if error.
 */
void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total;
	void	*result;

	total = nmemb * size;
	if (size != 0 && total / size != nmemb)
		return (NULL);
	result = malloc(total);
	if (!result)
		return (NULL);
	ft_bzero(result, total);
	return (result);
}
