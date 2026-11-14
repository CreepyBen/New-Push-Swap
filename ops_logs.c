#include "push_swap.h"

int	record_operation(t_push_swap *ps, t_op op)
{
	t_op	*grown;
	int		cap;
	int		i;

	if (ps->op_count == ps->op_capacity)
	{
		cap = 1024;
		if (ps->op_capacity > 0)
			cap = ps->op_capacity * 2;
		grown = malloc(sizeof(t_op) * (size_t)cap);
		if (!grown)
		{
			ps->error = 1;
			return (0);
		}
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

static void	put_str(const char *s, int idx)
{
	int	len;

	while (idx > 0)
	{
		while (*s)
			s++;
		s++;
		idx--;
	}
	len = 0;
	while (s[len])
		len++;
	write(2, s, len);
}

static void	put_num(long n, int width)
{
	char	c;

	if (n >= 10 || width > 1)
		put_num(n / 10, width - 1);
	c = (char)('0' + n % 10);
	write(2, &c, 1);
}

static void	put_counts(const t_push_swap *ps, int first, int last)
{
	int	op;
	int	i;
	int	n;

	put_str("[bench] ", 0);
	op = first;
	while (op <= last)
	{
		n = 0;
		i = 0;
		while (i < ps->op_count)
		{
			if (ps->ops[i] == (t_op)op)
				n++;
			i++;
		}
		put_str("sa\0sb\0ss\0pa\0pb\0ra\0rb\0rr\0rra\0rrb\0rrr", op);
		put_str(": ", 0);
		put_num(n, 1);
		put_str(" \0\n", op == last);
		op++;
	}
}

void	print_benchmark(const t_push_swap *ps)
{
	long	pct;

	pct = (long)(ps->disorder * 10000.0 + 0.5);
	put_str("[bench] disorder: ", 0);
	put_num(pct / 100, 1);
	put_str(".", 0);
	put_num(pct % 100, 2);
	put_str("%\n[bench] strategy: ", 0);
	put_str("Adaptive\0Simple\0Medium\0Complex", ps->strategy);
	put_str(" / ", 0);
	put_str("unused\0O(n^2)\0O(n\342\210\232n)\0O(n log n)",
		ps->selected_strategy);
	put_str("\n[bench] total_ops: ", 0);
	put_num(ps->op_count, 1);
	put_str("\n", 0);
	put_counts(ps, OP_SA, OP_PB);
	put_counts(ps, OP_RA, OP_RRR);
}
