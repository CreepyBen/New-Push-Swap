/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   s_adaptive.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/15 21:12:44 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:04:54 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	s_adaptive(t_push_swap *ps)
{
	if (!ps || ps->error)
		return ;
	if (ps->disorder < 0.2)
		ps->selected_strategy = STRAT_SIMPLE;
	else if (ps->disorder < 0.5)
		ps->selected_strategy = STRAT_MEDIUM;
	else
		ps->selected_strategy = STRAT_COMPLEX;
	if (ps->a.size <= 5)
		s_small(ps);
	else if (ps->selected_strategy == STRAT_SIMPLE)
		s_simple(ps);
	else if (ps->selected_strategy == STRAT_MEDIUM)
		s_medium(ps);
	else
		s_complex(ps);
}
