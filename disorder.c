/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disorder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:23 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:12:24 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

double	compute_disorder(const t_stack *stack)
{
	long	mistakes;
	long	pairs;
	int		i;
	int		j;

	if (!stack || stack->size < 2)
		return (0.0);
	mistakes = 0;
	i = 0;
	while (i < stack->size)
	{
		j = i + 1;
		while (j < stack->size)
		{
			if (stack_at(stack, i) > stack_at(stack, j))
				mistakes++;
			j++;
		}
		i++;
	}
	pairs = (long)stack->size * (stack->size - 1) / 2;
	return ((double)mistakes / (double)pairs);
}
