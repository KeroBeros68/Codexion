/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kebertra <kebertra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 19:57:37 by kebertra          #+#    #+#             */
/*   Updated: 2026/02/27 21:28:42 by kebertra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

/*
** heap_push
** Inserts a new node into the min-heap and restores the heap property
** by sifting the node up until its key is greater than or equal to its
** parent's key.
** The heap is fixed-capacity: if size has reached capacity, the node
** is silently dropped. This is intentional — each dongle is shared by
** exactly two coders, so the capacity is always 2 and can never be
** exceeded under normal operation.
**
** @param heap  Pointer to the heap to insert into.
** @param node  Node to insert, carrying a coder_id and a sort key.
*/
void	heap_push(t_heap *heap, t_heap_node node)
{
	int			i;
	int			parent;
	t_heap_node	tmp;

	if (heap->size >= heap->capacity)
		return ;
	heap->nodes[heap->size] = node;
	i = heap->size;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->nodes[parent].key > heap->nodes[i].key)
		{
			tmp = heap->nodes[parent];
			heap->nodes[parent] = heap->nodes[i];
			heap->nodes[i] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

/*
** heap_pop
** Removes and returns the root node (minimum key) from the heap.
** The last node is moved to the root position and size is decremented.
** With a fixed capacity of 2 there is at most one remaining node after
** removal, so no sift-down pass is needed.
** Returns a zero-initialized node if the heap is empty.
**
** @param heap  Pointer to the heap to pop from.
** @return      The node with the smallest key, or a zeroed node if empty.
*/
t_heap_node	heap_pop(t_heap *heap)
{
	t_heap_node	empty;
	t_heap_node	node;

	if (heap->size == 0)
	{
		ft_bzero(&empty, sizeof(empty));
		return (empty);
	}
	node = heap->nodes[0];
	heap->nodes[0] = heap->nodes[heap->size - 1];
	heap->size--;
	return (node);
}

/*
** heap_peek
** Returns the root node (minimum key) without modifying the heap.
** Used by acquire_dongles to check whether the calling coder is at the
** front of the wait queue before blocking on pthread_cond_wait.
** Returns a zero-initialized node if the heap is empty.
**
** @param heap  Pointer to the heap to inspect.
** @return      The node with the smallest key, or a zeroed node if empty.
*/
t_heap_node	heap_peek(t_heap *heap)
{
	t_heap_node	empty;

	if (heap->size == 0)
	{
		ft_bzero(&empty, sizeof(empty));
		return (empty);
	}
	return (heap->nodes[0]);
}
