NAME	=	minishell
SRCS	=	exec_func/exec_func.c tokenizer/tokenizer.c tokenizer/expand.c tokenizer/error.c\
			utils/strlcat.c utils/strlcpy.c utils/split.c parser.c\
			readline/readline.c
OBJS	=	$(SRCS:%.c=%.o)
CC  	=	gcc
CFLAGS	=	-Wall -Werror -Wextra
RLDIR	=	$(shell brew --prefix readline)

all:$(NAME)

$(NAME):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(RLDIR)/lib -I$(RLDIR)/include -o $(NAME) -lreadline

debug:$(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
