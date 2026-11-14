NAME = push_swap

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = main.c \
	parse.c \
	stack.c \
	ops.c \
	ops_logs.c \
	disorder.c \
	rank.c \
	moves.c \
	moves_exec.c \
	s_small.c \
	s_simple.c \
	s_medium.c \
	s_complex.c \
	s_adaptive.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c push_swap.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
