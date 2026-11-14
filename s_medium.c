#include "push_swap.h"

static int	nearest_below(const t_stack *stack, int limit)
{
	int	i;
	int	first;
	int	last;

	first = -1;
	last = -1;
	i = 0;
	while (i < stack->size)
	{
		if (stack_at(stack, i) < limit)
		{
			if (first < 0)
				first = i;
			last = i;
		}
		i++;
	}
	if (first < 0)
		return (-1);
	if (first <= stack->size - last)
		return (first);
	return (last);
}

static void	rotate_to(t_push_swap *ps, int index, t_op fwd, t_op rev)
{
	const t_stack	*stack;
	t_op			op;
	int				count;

	stack = &ps->a;
	if (fwd == OP_RB)
		stack = &ps->b;
	op = fwd;
	count = index;
	if (index > stack->size / 2)
	{
		op = rev;
		count = stack->size - index;
	}
	while (count > 0)
	{
		apply_operation(ps, op);
		count--;
	}
}

static void	push_chunks(t_push_swap *ps, int size)
{
	int	limit;
	int	index;

	limit = size;
	while (ps->a.size > 0 && !ps->error)
	{
		index = nearest_below(&ps->a, limit);
		if (index < 0)
			limit += size;
		else
		{
			rotate_to(ps, index, OP_RA, OP_RRA);
			apply_operation(ps, OP_PB);
			if (ps->b.size > 1 && stack_at(&ps->b, 0) < limit - size / 2)
				apply_operation(ps, OP_RB);
		}
	}
}

static void	pull_back(t_push_swap *ps)
{
	int	index;

	while (ps->b.size > 0 && !ps->error)
	{
		index = 0;
		while (index < ps->b.size - 1
			&& stack_at(&ps->b, index) != ps->b.size - 1)
			index++;
		rotate_to(ps, index, OP_RB, OP_RRB);
		apply_operation(ps, OP_PA);
	}
}

void	s_medium(t_push_swap *ps)
{
	int	k;
	int	size;

	if (!ps || ps->a.size < 2 || ps->error)
		return ;
	k = 1;
	while (k * k < ps->a.size)
		k++;
	size = (ps->a.size + k - 1) / k;
	push_chunks(ps, size);
	pull_back(ps);
}
