/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rank.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:40 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:12:41 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	get_rank(const int *values, int size, int index)
{
	int	j;
	int	rank;

	rank = 0;
	j = 0;
	while (j < size)
	{
		if (values[j] < values[index])
			rank++;
		j++;
	}
	return (rank);
}

int	rank_values(t_stack *stack)
{
	int	i;
	int	*values;

	if (!stack)
		return (0);
	if (stack->size <= 0)
		return (1);
	values = malloc(sizeof(int) * (size_t)stack->size);
	if (!values)
		return (0);
	i = 0;
	while (i < stack->size)
	{
		values[i] = stack_at(stack, i);
		i++;
	}
	i = 0;
	while (i < stack->size)
	{
		stack->data[(stack->head + i) % stack->capacity] = get_rank(values,
				stack->size, i);
		i++;
	}
	free(values);
	return (1);
}
