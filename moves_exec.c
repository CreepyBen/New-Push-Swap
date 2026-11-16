/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:28 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:03:41 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	do_ops(t_push_swap *ps, t_op op, int count)
{
	while (count > 0)
	{
		if (!apply_operation(ps, op))
		{
			ps->error = 1;
			return (0);
		}
		count--;
	}
	return (1);
}

static int	rotate_stacks(t_push_swap *ps, t_move *move)
{
	int	common;

	common = move->ra;
	if (move->rb < common)
		common = move->rb;
	move->ra -= common;
	move->rb -= common;
	if (!do_ops(ps, OP_RR, common))
		return (0);
	common = move->rra;
	if (move->rrb < common)
		common = move->rrb;
	move->rra -= common;
	move->rrb -= common;
	return (do_ops(ps, OP_RRR, common)
		&& do_ops(ps, OP_RA, move->ra)
		&& do_ops(ps, OP_RB, move->rb)
		&& do_ops(ps, OP_RRA, move->rra)
		&& do_ops(ps, OP_RRB, move->rrb));
}

void	execute_move(t_push_swap *ps, t_move move)
{
	t_op	push;

	if (!ps || ps->error || move.cost < 0)
		return ;
	if (!rotate_stacks(ps, &move))
		return ;
	push = OP_PB;
	if (move.direction == B_TO_A)
		push = OP_PA;
	do_ops(ps, push, 1);
}
