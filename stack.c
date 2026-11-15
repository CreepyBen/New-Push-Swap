/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 01:36:45 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:59:22 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	stack_init(t_stack *stack, int capacity)
{
	if (!stack || capacity < 0)
		return (0);
	stack->data = NULL;
	stack->head = 0;
	stack->size = 0;
	stack->capacity = capacity;
	if (capacity == 0)
		return (1);
	stack->data = malloc(sizeof(int) * (size_t)capacity);
	if (!stack->data)
	{
		stack->capacity = 0;
		return (0);
	}
	return (1);
}

void	stack_destroy(t_stack *stack)
{
	if (!stack)
		return ;
	free(stack->data);
	stack->data = NULL;
	stack->head = 0;
	stack->size = 0;
	stack->capacity = 0;
}

int	stack_at(const t_stack *stack, int position)
{
	if (!stack || !stack->data || stack->capacity <= 0)
		return (0);
	if (position < 0 || position >= stack->size)
		return (0);
	return (stack->data[(stack->head + position) % stack->capacity]);
}

int	stack_is_sorted(const t_stack *stack)
{
	int	index;

	if (!stack)
		return (0);
	index = 1;
	while (index < stack->size)
	{
		if (stack_at(stack, index - 1) > stack_at(stack, index))
			return (0);
		index++;
	}
	return (1);
}

void	ps_destroy(t_push_swap *ps)
{
	if (!ps)
		return ;
	stack_destroy(&ps->a);
	stack_destroy(&ps->b);
	free(ps->ops);
	ps->ops = NULL;
	ps->op_count = 0;
	ps->op_capacity = 0;
}
