/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   s_medium.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:49 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 22:04:01 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	do_op(t_push_swap *ps, t_op op)
{
	if (!ps->error && !apply_operation(ps, op))
		ps->error = 1;
}

static void	bring_chunk_top(t_push_swap *ps, int limit)
{
	int	top;
	int	bottom;

	top = 0;
	while (stack_at(&ps->a, top) > limit)
		top++;
	bottom = 1;
	while (stack_at(&ps->a, ps->a.size - bottom) > limit)
		bottom++;
	while (top > 0 && top <= bottom)
	{
		do_op(ps, OP_RA);
		top--;
	}
	while (top > bottom && bottom-- > 0)
		do_op(ps, OP_RRA);
}

static void	push_chunks(t_push_swap *ps, int chunk)
{
	int	pushed;
	int	rank;

	pushed = 0;
	while (ps->a.size > 0 && !ps->error)
	{
		bring_chunk_top(ps, pushed + chunk);
		rank = stack_at(&ps->a, 0);
		do_op(ps, OP_PB);
		if (rank < pushed)
			do_op(ps, OP_RB);
		pushed++;
	}
}

static void	rotate_b_to(t_push_swap *ps, int value)
{
	int	pos;
	int	down;

	pos = 0;
	while (pos < ps->b.size && stack_at(&ps->b, pos) != value)
		pos++;
	down = ps->b.size - pos;
	while (pos > 0 && pos <= ps->b.size / 2)
	{
		do_op(ps, OP_RB);
		pos--;
	}
	while (pos > ps->b.size / 2 && down-- > 0)
		do_op(ps, OP_RRB);
}

void	s_medium(t_push_swap *ps)
{
	int	chunk;

	if (!ps || ps->error || ps->a.size < 2)
		return ;
	chunk = 1;
	while ((chunk + 1) * (chunk + 1) <= ps->a.size)
		chunk++;
	push_chunks(ps, chunk * 2);
	while (ps->b.size > 0 && !ps->error)
	{
		rotate_b_to(ps, ps->b.size - 1);
		do_op(ps, OP_PA);
	}
}
