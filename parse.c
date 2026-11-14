#include "push_swap.h"

static int	read_int(const char **s, int *out)
{
	long	v;
	int		neg;

	while (**s == ' ')
		(*s)++;
	if (!**s)
		return (-1);
	neg = (**s == '-');
	if (**s == '-' || **s == '+')
		(*s)++;
	if (**s < '0' || **s > '9')
		return (0);
	v = 0;
	while (**s >= '0' && **s <= '9' && v <= INT32_MAX + 1L)
	{
		v = v * 10 + (**s - '0');
		(*s)++;
	}
	if (neg)
		v = -v;
	if ((**s && **s != ' ') || v < INT32_MIN || v > INT32_MAX)
		return (0);
	*out = (int)v;
	return (1);
}

static int	parse_flag(t_push_swap *ps, const char *arg)
{
	const char	*name;
	int			idx;
	int			i;

	name = "--adaptive\0--simple\0--medium\0--complex\0--bench";
	idx = 0;
	while (idx < 5)
	{
		i = 0;
		while (name[i] && name[i] == arg[i])
			i++;
		if (!name[i] && !arg[i])
			break ;
		while (*name)
			name++;
		name++;
		idx++;
	}
	if (idx == 4)
		ps->bench = 1;
	else if (idx < 4)
		ps->strategy = (t_strat)idx;
	return (idx < 5);
}

static int	scan_args(int argc, char **argv, int *values)
{
	int			i;
	int			n;
	int			rc;
	int			tmp;
	const char	*s;

	i = 0;
	n = 0;
	while (i < argc)
	{
		s = argv[i++];
		rc = read_int(&s, &tmp);
		if (rc < 1)
			return (-1);
		while (rc == 1)
		{
			if (values)
				values[n] = tmp;
			n++;
			rc = read_int(&s, &tmp);
		}
		if (rc == 0)
			return (-1);
	}
	return (n);
}

int	validate_values(const int *values, int count)
{
	int	i;
	int	j;

	i = 0;
	while (i < count)
	{
		j = i + 1;
		while (j < count)
		{
			if (values[i] == values[j])
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

int	parse_args(t_push_swap *ps, int argc, char **argv)
{
	int	i;
	int	count;

	i = 1;
	while (i < argc && argv[i][0] == '-' && argv[i][1] == '-')
	{
		if (!parse_flag(ps, argv[i]))
			return (0);
		i++;
	}
	count = scan_args(argc - i, argv + i, NULL);
	if (count < 0 || !stack_init(&ps->a, count))
		return (0);
	if (!stack_init(&ps->b, count))
		return (0);
	scan_args(argc - i, argv + i, ps->a.data);
	ps->a.size = count;
	return (validate_values(ps->a.data, count));
}
