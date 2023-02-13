/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/29 16:28:10 by user              #+#    #+#             */
/*   Updated: 2023/02/12 17:33:21 by user             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef minishell_h
# define minishell_h

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

/**** token *****/

#define IN 0
#define OUT 1
#define APPEND 2
#define HEREDOC 3

typedef struct s_token t_token;

extern bool    syntax_error;

typedef enum e_token_kind {
    TK_WORD,
    TK_RESERVED, // いる？
    TK_REDIRECT,
    TK_OP,
    TK_EOF,
}            t_token_kind;

struct s_token {
    char         *word;
    t_token_kind kind;
    t_token      *next;
};

/**** simple command node ****/

enum e_node_kind {
	ND_SIMPLE_CMD,
};

typedef enum e_node_kind t_node_kind;
typedef struct s_redirect t_redirect;

typedef struct s_node	t_node;

// typedef enum e_redirect_type
// {
// 	IN,
// 	OUT,
// } t_redirect_type;

typedef struct	s_redirect
{
	int					type;
	char				*file_path;
	int					stashed_fd;
	int					redirectfile;
	t_redirect			*next;
	t_redirect			*before;
}				t_redirect;

typedef struct s_command
{
	t_token		 	*args;
	t_redirect		**redirect;
	int				now_in;
	int				now_out;
}	t_command;

struct s_node {
	t_command	*command;
	t_node_kind	kind;
	t_node		*next;
};

t_token *tokenizer(char *line);
int     interpret(t_command *command);
size_t	ft_strlcat(char *dst, const char *src, size_t dstsize);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
char	**ft_split(char const *s, char c);
int	    exec(t_command *command);
int     abusolute_path(char *line);
bool    is_metacharactert(char c);
void    expand(t_token *tok);
void    tokenize_error(const char *location, char **rest, char *line);
void	free_token(t_token *head);

void    fatal_error(const char *msg) __attribute__((noreturn));


t_node	*parse(t_token *tok);

#endif
