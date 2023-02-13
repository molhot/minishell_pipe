#include <stdio.h>
#include "../minishell.h"
void	free_token(t_token *head)
{
	t_token	*itr;
	t_token	*next;

	itr = head;
	next = itr->next;
	printf("%p", itr);
	printf("%p", next);
	while (next != NULL)
	{
		//printf("%p\n", itr);
		free(itr->word);
		free(itr);
		itr = next;
		next = itr->next;
	}
	free(itr->word);
	free(itr);
}

void	test_tokenizer(void)
{
	char	*line = "echo hoge | echo huga";
	t_token *t = tokenizer(line);
	t_token *itr = t;
	while(itr->next != NULL)
	{
		printf("%s\n", itr->word);
		itr = itr->next;
	}
	free_token(t);	
}

__attribute__((destructor))
static void destructor()
{
	system("leaks -q a.out");
}

int	main(void)
{
	test_tokenizer();
}