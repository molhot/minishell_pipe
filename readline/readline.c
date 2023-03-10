#include "../minishell.h"
#include <stdio.h>

/*__attribute__((destructor))
static void destructor()
{
	system("leaks -q minishell");
}*/

int main()
{
	char	  *line;
	t_token	*tok;
  	t_node  *node;
	//t_node	*fnode;

	rl_outstream = stderr;
	while (1)
	{
		line = readline("minishell$ ");
		if (line == NULL)
			break;
		if (*line)
			add_history(line);
		if (line[0] == '/' || line[0] == '.')
			abusolute_path(line);
		else
		{
			tok = tokenizer(line);
			expand(tok);
			node = parse(tok);
			exec(node);
			//exec_pipeline(node);
			if (tok != NULL)
				free_token(tok);
		}
		free(line);
	}
	exit(0);
}
