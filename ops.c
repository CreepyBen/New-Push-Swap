/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 01:13:14 by ben               #+#    #+#             */
/*   Updated: 2026/11/14 01:55:53 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	op_swap(t_push_swap *ps, t_op op)
{
	t_stack	*stack;
	int		temp;

	stack = &ps->a;
	if (op == OP_SB)
		stack = &ps->b;
	if (stack->size < 2)
		return ;
	temp = stack->data[stack->head];
	stack->data[stack->head] = stack->data[
		(stack->head + 1) % stack->capacity];
	stack->data[(stack->head + 1) % stack->capacity] = temp;
}

static void	op_push(t_push_swap *ps, t_op op)
{
	t_stack	*src;
	t_stack	*dst;
	int		value;

	src = &ps->a;
	dst = &ps->b;
	if (op == OP_PA)
	{
		src = &ps->b;
		dst = &ps->a;
	}
	if (src->size == 0 || dst->size >= dst->capacity)
		return ;
	value = src->data[src->head];
	src->head = (src->head + 1) % src->capacity;
	src->size--;
	dst->head = (dst->head - 1 + dst->capacity) % dst->capacity;
	dst->data[dst->head] = value;
	dst->size++;
}

static void	op_rotate(t_push_swap *ps, t_op op)
{
	t_stack	*stack;
	int		i;
	int		value;

	i = 0;
	while (i < 2)
	{
		if (op == OP_RR || (op == OP_RA && i == 0)
			|| (op == OP_RB && i == 1))
		{
			stack = &ps->a;
			if (i == 1)
				stack = &ps->b;
			if (stack->size > 1)
			{
				value = stack->data[stack->head];
				stack->head = (stack->head + 1) % stack->capacity;
				stack->data[(stack->head + stack->size - 1)
					% stack->capacity] = value;
			}
		}
		i++;
	}
}

static void	op_reverse_rotate(t_push_swap *ps, t_op op)
{
	t_stack	*stack;
	int		i;
	int		value;

	i = 0;
	while (i < 2)
	{
		if (op == OP_RRR || (op == OP_RRA && i == 0)
			|| (op == OP_RRB && i == 1))
		{
			stack = &ps->a;
			if (i == 1)
				stack = &ps->b;
			if (stack->size > 1)
			{
				value = stack->data[(stack->head + stack->size - 1)
					% stack->capacity];
				stack->head = (stack->head - 1
						+ stack->capacity) % stack->capacity;
				stack->data[stack->head] = value;
			}
		}
		i++;
	}
}

int	apply_operation(t_push_swap *ps, t_op op)
{
	if (!ps || op < OP_SA || op >= OP_COUNT)
		return (0);
	if (op == OP_SS)
	{
		op_swap(ps, OP_SA);
		op_swap(ps, OP_SB);
	}
	else if (op == OP_RR || op == OP_RA || op == OP_RB)
		op_rotate(ps, op);
	else if (op == OP_RRR || op == OP_RRA || op == OP_RRB)
		op_reverse_rotate(ps, op);
	else if (op == OP_SA || op == OP_SB)
		op_swap(ps, op);
	else if (op == OP_PA || op == OP_PB)
		op_push(ps, op);
	if (!record_operation(ps, op))
		return (0);
	return (1);
}
