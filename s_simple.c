/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   s_simple.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:52 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:13:37 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	push_to_b(t_push_swap *ps)
{
	if (ps->a.size > 3)
		apply_operation(ps, OP_PB);
	if (ps->a.size > 3)
		apply_operation(ps, OP_PB);
	while (ps->a.size > 3 && !ps->error)
		execute_move(ps, select_cheapest_move(ps, A_TO_B));
}

static void	pull_back(t_push_swap *ps)
{
	while (ps->b.size > 0 && !ps->error)
		execute_move(ps, select_cheapest_move(ps, B_TO_A));
}

static void	min_to_top(t_push_swap *ps)
{
	int	i;
	int	min;

	min = 0;
	i = 1;
	while (i < ps->a.size)
	{
		if (stack_at(&ps->a, i) < stack_at(&ps->a, min))
			min = i;
		i++;
	}
	while (min > 0 && min <= ps->a.size / 2)
	{
		apply_operation(ps, OP_RA);
		min--;
	}
	while (min > ps->a.size / 2 && min < ps->a.size)
	{
		apply_operation(ps, OP_RRA);
		min++;
	}
}

void	s_simple(t_push_swap *ps)
{
	if (!ps || ps->error)
		return ;
	if (ps->a.size <= 3)
	{
		s_small(ps);
		return ;
	}
	push_to_b(ps);
	s_small(ps);
	pull_back(ps);
	min_to_top(ps);
}
