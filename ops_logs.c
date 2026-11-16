/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_logs.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:32 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:04:07 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	record_operation(t_push_swap *ps, t_op op)
{
	t_op	*grown;
	int		cap;
	int		i;

	if (!ps)
		return (0);
	if (ps->op_count >= ps->op_capacity)
	{
		cap = 2 * ps->op_capacity + 256;
		grown = malloc(sizeof(t_op) * (size_t)cap);
		if (!grown)
			return (0);
		i = -1;
		while (++i < ps->op_count)
			grown[i] = ps->ops[i];
		free(ps->ops);
		ps->ops = grown;
		ps->op_capacity = cap;
	}
	ps->ops[ps->op_count++] = op;
	return (1);
}

void	emit_operations(const t_push_swap *ps)
{
	const char	*names;
	const char	*name;
	char		buf[4096];
	int			pos;
	int			i;

	names = "sa\0\0sb\0\0ss\0\0pa\0\0pb\0\0ra\0\0rb\0\0rr\0\0rra\0rrb\0rrr";
	pos = 0;
	i = 0;
	while (ps && i < ps->op_count)
	{
		name = names + 4 * ps->ops[i++];
		buf[pos++] = name[0];
		buf[pos++] = name[1];
		if (name[2])
			buf[pos++] = name[2];
		buf[pos++] = '\n';
		if (pos > 4090)
		{
			write(1, buf, pos);
			pos = 0;
		}
	}
	write(1, buf, pos);
}

static void	put(const char *s, long n)
{
	char	buf[21];
	int		i;

	while (*s)
		write(2, s++, 1);
	if (n < 0)
		return ;
	i = 20;
	buf[i] = '0' + n % 10;
	n /= 10;
	while (n > 0)
	{
		buf[--i] = '0' + n % 10;
		n /= 10;
	}
	write(2, buf + i, 21 - i);
}

static void	put_ops(const t_push_swap *ps, int from, int to)
{
	const char	*names;
	long		count;
	int			i;
	int			j;

	names = "sa\0\0sb\0\0ss\0\0pa\0\0pb\0\0ra\0\0rb\0\0rr\0\0rra\0rrb\0rrr";
	put("[bench]", -1);
	i = from;
	while (i <= to)
	{
		count = 0;
		j = 0;
		while (j < ps->op_count)
			count += (ps->ops[j++] == (t_op)i);
		put(" ", -1);
		put(names + 4 * i, -1);
		put(":", -1);
		put(" ", count);
		i++;
	}
	put("\n", -1);
}

void	print_benchmark(const t_push_swap *ps)
{
	const char	*name;
	long		pct;

	name = "Adaptive\0Simple\0\0\0Medium\0\0\0Complex";
	pct = (long)(ps->disorder * 10000.0 + 0.5);
	put("[bench] disorder: ", pct / 100);
	put(".", pct % 100 / 10);
	put("", pct % 10);
	put("%\n[bench] strategy: ", -1);
	put(name + 9 * ps->strategy, -1);
	put(" / O(n", -1);
	if (ps->selected_strategy == STRAT_SIMPLE)
		put("^2)\n", -1);
	else if (ps->selected_strategy == STRAT_MEDIUM)
		put("\xe2\x88\x9a" "n)\n", -1);
	else
		put(" log n)\n", -1);
	put("[bench] total_ops: ", ps->op_count);
	put("\n", -1);
	put_ops(ps, OP_SA, OP_PB);
	put_ops(ps, OP_RA, OP_RRR);
}
