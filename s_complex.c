/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   s_complex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:46 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:05:03 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	get_max_bits(int size)
{
	int	bits;
	int	max;

	bits = 0;
	max = size - 1;
	while (max > 0)
	{
		bits++;
		max >>= 1;
	}
	return (bits);
}

static void	sort_bit(t_push_swap *ps, int bit, int size)
{
	int	i;
	int	value;

	i = 0;
	while (i < size && !ps->error)
	{
		value = stack_at(&ps->a, 0);
		if ((value >> bit) & 1)
		{
			if (!apply_operation(ps, OP_RA))
				ps->error = 1;
		}
		else if (!apply_operation(ps, OP_PB))
			ps->error = 1;
		i++;
	}
	while (ps->b.size > 0 && !ps->error)
	{
		if (!apply_operation(ps, OP_PA))
			ps->error = 1;
	}
}

void	s_complex(t_push_swap *ps)
{
	int	bit;
	int	size;
	int	max_bits;

	if (!ps || ps->a.size < 2 || ps->error)
		return ;
	size = ps->a.size;
	max_bits = get_max_bits(size);
	bit = 0;
	while (bit < max_bits && !ps->error && !stack_is_sorted(&ps->a))
	{
		sort_bit(ps, bit, size);
		bit++;
	}
}
