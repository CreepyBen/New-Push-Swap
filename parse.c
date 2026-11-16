/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:37 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:04:30 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	next_int(const char **s, int *out)
{
	const char	*p;
	long		n;
	int			neg;

	p = *s;
	while (*p == ' ' || (*p >= '\t' && *p <= '\r'))
		p++;
	*s = p;
	if (!*p)
		return (0);
	neg = (*p == '-');
	if (*p == '-' || *p == '+')
		p++;
	if (*p < '0' || *p > '9')
		return (-1);
	n = 0;
	while (*p >= '0' && *p <= '9' && n <= 2147483648L)
		n = n * 10 + (*p++ - '0');
	*s = p;
	if (n - neg > INT32_MAX || (*p && *p != ' ' && (*p < '\t' || *p > '\r')))
		return (-1);
	*out = (int)(n * (1 - 2 * neg));
	return (1);
}

static int	parse_option(t_push_swap *ps, const char *arg)
{
	const char	*names;
	int			k;
	int			i;

	if (arg[0] != '-' || arg[1] != '-')
		return (0);
	names = "--bench\0\0\0\0--simple\0\0\0"
		"--medium\0\0\0--complex\0\0--adaptive";
	k = 0;
	while (k < 5)
	{
		i = 0;
		while (arg[i] && arg[i] == names[11 * k + i])
			i++;
		if (arg[i] == names[11 * k + i])
			break ;
		k++;
	}
	if (k == 5 || (k == 0 && ps->bench))
		return (-1);
	if (k == 0)
		ps->bench = 1;
	else
		ps->strategy = (t_strat)(k % 4);
	return (1 + (k > 0));
}

static int	parse_arg(t_push_swap *ps, const char *arg, int *strat)
{
	int	opt;
	int	start;
	int	status;

	opt = parse_option(ps, arg);
	if (opt < 0 || (opt == 2 && (*strat)++))
		return (0);
	if (opt > 0)
		return (1);
	start = ps->a.size;
	status = next_int(&arg, &ps->a.data[ps->a.size]);
	while (status > 0)
	{
		ps->a.size++;
		status = next_int(&arg, &ps->a.data[ps->a.size]);
	}
	return (status == 0 && ps->a.size > start);
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
	int			i;
	int			cap;
	int			strat;
	const char	*p;

	cap = 1;
	i = 0;
	while (++i < argc)
	{
		p = argv[i];
		while (*p++)
			cap++;
	}
	if (!stack_init(&ps->a, cap) || !stack_init(&ps->b, cap))
		return (0);
	strat = 0;
	i = 0;
	while (++i < argc)
	{
		if (!parse_arg(ps, argv[i], &strat))
			return (0);
	}
	return (validate_values(ps->a.data, ps->a.size));
}
