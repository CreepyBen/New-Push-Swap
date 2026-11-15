/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:30 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 22:03:36 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	set_rotation(t_move *move, int index, int size, int is_a)
{
	int	fwd;
	int	rev;

	fwd = 0;
	rev = 0;
	if (index <= size / 2)
		fwd = index;
	else
		rev = size - index;
	if (is_a)
	{
		move->ra = fwd;
		move->rra = rev;
	}
	else
	{
		move->rb = fwd;
		move->rrb = rev;
	}
}

static void	set_move_cost(t_move *move)
{
	int	forward;
	int	reverse;

	forward = move->ra;
	if (move->rb > forward)
		forward = move->rb;
	reverse = move->rra;
	if (move->rrb > reverse)
		reverse = move->rrb;
	move->cost = forward + reverse;
}

int	find_target_position(const t_stack *destination, int value,
		t_transfer direction)
{
	int	i;
	int	current;
	int	next;

	if (!destination || destination->size == 0)
		return (0);
	i = 0;
	while (i < destination->size)
	{
		current = stack_at(destination, i);
		next = stack_at(destination, (i + 1) % destination->size);
		if (direction == B_TO_A && current < value && value < next)
			return (i + 1);
		if (direction == B_TO_A && current > next
			&& (value > current || value < next))
			return ((i + 1) % destination->size);
		if (direction == A_TO_B && current > value && value > next)
			return (i + 1);
		if (direction == A_TO_B && current < next
			&& (value < current || value > next))
			return ((i + 1) % destination->size);
		i++;
	}
	return (0);
}

t_move	calculate_move(const t_push_swap *ps, t_transfer dir, int idx)
{
	t_move			move;
	const t_stack	*src;
	const t_stack	*dst;

	move.direction = dir;
	move.source_index = idx;
	move.target_index = 0;
	move.cost = -1;
	set_rotation(&move, 0, 0, 1);
	set_rotation(&move, 0, 0, 0);
	src = &ps->a;
	dst = &ps->b;
	if (dir == B_TO_A)
	{
		src = &ps->b;
		dst = &ps->a;
	}
	if (idx < 0 || idx >= src->size)
		return (move);
	move.target_index = find_target_position(dst, stack_at(src, idx), dir);
	set_rotation(&move, idx, src->size, dir == A_TO_B);
	set_rotation(&move, move.target_index, dst->size, dir == B_TO_A);
	set_move_cost(&move);
	return (move);
}

t_move	select_cheapest_move(const t_push_swap *ps, t_transfer direction)
{
	t_move	best;
	t_move	candidate;
	int		i;
	int		size;

	size = ps->a.size;
	if (direction == B_TO_A)
		size = ps->b.size;
	best = calculate_move(ps, direction, 0);
	i = 1;
	while (i < size && best.cost > 0)
	{
		candidate = calculate_move(ps, direction, i);
		if (candidate.cost < best.cost)
			best = candidate;
		i++;
	}
	return (best);
}
