/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 01:07:20 by ben               #+#    #+#             */
/*   Updated: 2026/11/16 01:07:23 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stdlib.h>
# include <unistd.h>

typedef enum e_op
{
	OP_SA,
	OP_SB,
	OP_SS,
	OP_PA,
	OP_PB,
	OP_RA,
	OP_RB,
	OP_RR,
	OP_RRA,
	OP_RRB,
	OP_RRR,
	OP_COUNT
}				t_op;

typedef enum e_strat
{
	STRAT_ADAPTIVE,
	STRAT_SIMPLE,
	STRAT_MEDIUM,
	STRAT_COMPLEX
}				t_strat;

typedef enum e_transfer
{
	A_TO_B,
	B_TO_A
}				t_transfer;

typedef struct s_stack
{
	int			*data;
	int			head;
	int			size;
	int			capacity;
}				t_stack;

typedef struct s_push_swap
{
	t_stack		a;
	t_stack		b;
	t_op		*ops;
	int			op_count;
	int			op_capacity;
	t_strat		strategy;
	t_strat		selected_strategy;
	double		disorder;
	int			bench;
	int			error;
}				t_push_swap;

typedef struct s_move
{
	t_transfer	direction;
	int			source_index;
	int			target_index;
	int			ra;
	int			rb;
	int			rra;
	int			rrb;
	int			cost;
}				t_move;

int				parse_args(t_push_swap *ps, int argc, char **argv);
int				validate_values(const int *values, int count);
void			ps_destroy(t_push_swap *ps);
int				stack_init(t_stack *stack, int capacity);
void			stack_destroy(t_stack *stack);
int				stack_at(const t_stack *stack, int position);
int				stack_is_sorted(const t_stack *stack);
int				apply_operation(t_push_swap *ps, t_op op);
int				record_operation(t_push_swap *ps, t_op op);
void			emit_operations(const t_push_swap *ps);
void			print_benchmark(const t_push_swap *ps);
double			compute_disorder(const t_stack *stack);
int				rank_values(t_stack *stack);
void			s_small(t_push_swap *ps);
void			s_simple(t_push_swap *ps);
void			s_medium(t_push_swap *ps);
void			s_complex(t_push_swap *ps);
void			s_adaptive(t_push_swap *ps);
void			run_strategy(t_push_swap *ps);
int				find_target_position(const t_stack *destination, int value,
					t_transfer direction);
t_move			calculate_move(const t_push_swap *ps, t_transfer direction,
					int source_index);
t_move			select_cheapest_move(const t_push_swap *ps,
					t_transfer direction);
void			execute_move(t_push_swap *ps, t_move move);

#endif
