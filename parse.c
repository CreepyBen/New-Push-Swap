/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:37 by ben               #+#    #+#             */
/*   Updated: 2026/11/15 22:03:10 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	str_eq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

static int	parse_int(const char *s, int *out)
{
	long	n;
	int		sign;

	sign = 1 - 2 * (*s == '-');
	if (*s == '-' || *s == '+')
		s++;
	if (*s < '0' || *s > '9')
		return (0);
	n = 0;
	while (*s >= '0' && *s <= '9')
	{
		n = n * 10 + (*s++ - '0');
		if (n > 2147483648L)
			return (0);
	}
	n *= sign;
	if (*s != '\0' || n > INT32_MAX || n < INT32_MIN)
		return (0);
	*out = (int)n;
	return (1);
}

static int	parse_option(t_push_swap *ps, const char *arg)
{
	if (arg[0] != '-' || arg[1] != '-')
		return (0);
	if (str_eq(arg, "--bench") && !ps->bench)
		ps->bench = 1;
	else if (str_eq(arg, "--simple"))
		ps->strategy = STRAT_SIMPLE;
	else if (str_eq(arg, "--medium"))
		ps->strategy = STRAT_MEDIUM;
	else if (str_eq(arg, "--complex"))
		ps->strategy = STRAT_COMPLEX;
	else if (str_eq(arg, "--adaptive"))
		ps->strategy = STRAT_ADAPTIVE;
	else
		return (-1);
	return (1 + !str_eq(arg, "--bench"));
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
	int	opt;
	int	strat;

	if (!stack_init(&ps->a, argc) || !stack_init(&ps->b, argc))
		return (0);
	strat = 0;
	i = 1;
	while (i < argc)
	{
		opt = parse_option(ps, argv[i]);
		if (opt < 0 || (opt == 2 && strat++))
			return (0);
		if (opt == 0 && !parse_int(argv[i], &ps->a.data[ps->a.size++]))
			return (0);
		i++;
	}
	return (validate_values(ps->a.data, ps->a.size));
}
