/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ben <ben@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 01:30:57 by ben               #+#    #+#             */
/*   Updated: 2026/11/14 19:12:35 by ben              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>

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

/* parse.c: Parse options and stack values; return failure for invalid input. */
int				parse_args(t_push_swap *ps, int argc, char **argv);
/* parse.c: Reject duplicate values in the parsed input. */
int				validate_values(const int *values, int count);

/* stack.c: Free every allocation owned by the program state. */
void			ps_destroy(t_push_swap *ps);
/* stack.c: Allocate and initialize a stack with the requested capacity. */
int				stack_init(t_stack *stack, int capacity);
/* stack.c: Free the stack's backing storage. */
void			stack_destroy(t_stack *stack);
/* stack.c: Return the value at a logical offset from the stack top. */
int				stack_at(const t_stack *stack, int position);
/* stack.c: Return nonzero when the stack is in ascending order. */
int				stack_is_sorted(const t_stack *stack);

/* ops.c: Apply one push_swap operation and record it. */
int				apply_operation(t_push_swap *ps, t_op op);

/* ops_logs.c: Append an operation to the growable operation log. */
int				record_operation(t_push_swap *ps, t_op op);
/* ops_logs.c: Print the operation log to standard output. */
void			emit_operations(const t_push_swap *ps);

/* ops_logs.c: Print disorder, strategy,
	and operation counts to standard error. */
void			print_benchmark(const t_push_swap *ps);

/* disorder.c: Measure inversion disorder before sorting. */
double			compute_disorder(const t_stack *stack);
/* ranks.c: Replace values with order-preserving ranks from 0 to n - 1. */
int				rank_values(t_stack *stack);

/* s_small.c*/
void			s_small(t_push_swap *ps);

/* s_simple.c: Run the required quadratic baseline strategy. */
void			s_simple(t_push_swap *ps);

/* s_medium.c: Run the required medium-complexity strategy. */
void			s_medium(t_push_swap *ps);

/* s_complex.c: Run binary radix sort on compressed ranks. */
void			s_complex(t_push_swap *ps);

/* s_adaptive.c: Select a strategy based on measured disorder. */
void			s_adaptive(t_push_swap *ps);

/* main.c: Dispatch the requested strategy, handling trivial inputs. */
void			run_strategy(t_push_swap *ps);

/* moves.c: Find the destination index that preserves circular order. */
int				find_target_position(const t_stack *destination, int value,
					t_transfer direction);
/* moves.c: Calculate rotations and cost for one candidate transfer. */
t_move			calculate_move(const t_push_swap *ps, t_transfer direction,
					int source_index);

t_move			select_cheapest_move(const t_push_swap *ps,
					t_transfer direction);
/* moves.c: Rotate both stacks as needed, then push in the chosen direction. */
void			execute_move(t_push_swap *ps, t_move move);

#endif
