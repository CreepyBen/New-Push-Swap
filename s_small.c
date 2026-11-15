/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   s_small.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:54 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:12:55 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	sort_three(t_push_swap *ps)
{
	int	top;
	int	mid;
	int	bot;

	top = stack_at(&ps->a, 0);
	mid = stack_at(&ps->a, 1);
	bot = stack_at(&ps->a, 2);
	if (top > mid && top > bot)
		apply_operation(ps, OP_RA);
	else if (mid > top && mid > bot)
		apply_operation(ps, OP_RRA);
	if (stack_at(&ps->a, 0) > stack_at(&ps->a, 1))
		apply_operation(ps, OP_SA);
}

static int	min_index(const t_stack *stack)
{
	int	i;
	int	min;

	min = 0;
	i = 1;
	while (i < stack->size)
	{
		if (stack_at(stack, i) < stack_at(stack, min))
			min = i;
		i++;
	}
	return (min);
}

static void	push_min(t_push_swap *ps)
{
	int	index;
	int	size;

	index = min_index(&ps->a);
	size = ps->a.size;
	while (index > 0 && index <= size / 2)
	{
		apply_operation(ps, OP_RA);
		index--;
	}
	while (index > size / 2 && index < size)
	{
		apply_operation(ps, OP_RRA);
		index++;
	}
	apply_operation(ps, OP_PB);
}

void	s_small(t_push_swap *ps)
{
	if (ps->a.size == 2 && stack_at(&ps->a, 0) > stack_at(&ps->a, 1))
		apply_operation(ps, OP_SA);
	else if (ps->a.size == 3)
		sort_three(ps);
	else if (ps->a.size == 4 || ps->a.size == 5)
	{
		while (ps->a.size > 3)
			push_min(ps);
		sort_three(ps);
		while (ps->b.size > 0)
			apply_operation(ps, OP_PA);
	}
}
