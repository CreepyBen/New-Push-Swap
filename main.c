/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:26 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 21:19:17 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	init_ps(t_push_swap *ps)
{
	stack_init(&ps->a, 0);
	stack_init(&ps->b, 0);
	ps->ops = NULL;
	ps->op_count = 0;
	ps->op_capacity = 0;
	ps->strategy = STRAT_ADAPTIVE;
	ps->selected_strategy = STRAT_ADAPTIVE;
	ps->disorder = 0.0;
	ps->bench = 0;
	ps->error = 0;
}

static int	merge_ops(int top, int op)
{
	const char	*rules;
	int			i;

	rules = "aazbbzcczdezedzfizifzgjzjgzhkzkhz"
		"abcbacfghgfhijkjikhighjfkfjkgi";
	i = 0;
	while (rules[i])
	{
		if (rules[i] - 'a' == top && rules[i + 1] - 'a' == op)
			return (rules[i + 2] - 'a');
		i += 3;
	}
	return (-1);
}

static void	simplify_ops(t_push_swap *ps)
{
	int	n;
	int	i;
	int	res;
	int	op;

	n = 0;
	i = 0;
	while (i < ps->op_count)
	{
		op = ps->ops[i++];
		while (n > 0 && op < OP_COUNT)
		{
			res = merge_ops(ps->ops[n - 1], op);
			if (res < 0)
				break ;
			n--;
			op = res;
		}
		if (op < OP_COUNT)
			ps->ops[n++] = op;
	}
	ps->op_count = n;
}

void	run_strategy(t_push_swap *ps)
{
	ps->selected_strategy = ps->strategy;
	if (ps->strategy == STRAT_ADAPTIVE)
		ps->selected_strategy = STRAT_SIMPLE;
	if (ps->a.size < 2)
		return ;
	ps->disorder = compute_disorder(&ps->a);
	if (stack_is_sorted(&ps->a))
		return ;
	if (!rank_values(&ps->a))
	{
		ps->error = 1;
		return ;
	}
	if (ps->strategy == STRAT_SIMPLE)
		s_simple(ps);
	else if (ps->strategy == STRAT_MEDIUM)
		s_medium(ps);
	else if (ps->strategy == STRAT_COMPLEX)
		s_complex(ps);
	else
		s_adaptive(ps);
}

int	main(int argc, char **argv)
{
	t_push_swap	ps;

	init_ps(&ps);
	if (!parse_args(&ps, argc, argv))
	{
		ps_destroy(&ps);
		write(2, "Error\n", 6);
		return (1);
	}
	run_strategy(&ps);
	if (ps.error)
	{
		ps_destroy(&ps);
		write(2, "Error\n", 6);
		return (1);
	}
	simplify_ops(&ps);
	emit_operations(&ps);
	if (ps.bench)
		print_benchmark(&ps);
	ps_destroy(&ps);
	return (0);
}
