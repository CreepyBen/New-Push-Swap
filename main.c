
#include "push_swap.h"

static void	init_ps(t_push_swap *ps)
{
	unsigned char	*bytes;
	size_t			i;

	bytes = (unsigned char *)ps;
	i = 0;
	while (i < sizeof(*ps))
	{
		bytes[i] = 0;
		i++;
	}
}

static int	fail(t_push_swap *ps)
{
	ps_destroy(ps);
	write(2, "Error\n", 6);
	return (1);
}

void	emit_operations(const t_push_swap *ps)
{
	const char	*name;
	int			i;
	int			skip;
	int			len;

	i = 0;
	while (i < ps->op_count)
	{
		name = "sa\0sb\0ss\0pa\0pb\0ra\0rb\0rr\0rra\0rrb\0rrr";
		skip = ps->ops[i];
		while (skip > 0)
		{
			while (*name)
				name++;
			name++;
			skip--;
		}
		len = 0;
		while (name[len])
			len++;
		write(1, name, len);
		write(1, "\n", 1);
		i++;
	}
}

void	run_strategy(t_push_swap *ps)
{
	ps->selected_strategy = ps->strategy;
	if (ps->strategy == STRAT_ADAPTIVE)
		ps->selected_strategy = STRAT_SIMPLE;
	if (ps->a.size < 2 || stack_is_sorted(&ps->a))
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

	if (argc < 2)
		return (0);
	init_ps(&ps);
	if (!parse_args(&ps, argc, argv))
		return (fail(&ps));
	ps.disorder = compute_disorder(&ps.a);
	run_strategy(&ps);
	if (ps.error)
		return (fail(&ps));
	emit_operations(&ps);
	if (ps.bench && ps.a.size > 0)
		print_benchmark(&ps);
	ps_destroy(&ps);
	return (0);
}