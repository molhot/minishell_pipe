#include "../minishell.h"
#include <ctype.h>

void fatal_error(const char *msg) __attribute__((noreturn));

/*void    fatal_error(const char *msg)
{
    dprintf(STDERR_FILENO, "Fatal Error: %s", msg);
    exit(1);
}*/

char *searchpath(const char *filename)
{
    char path[PATH_MAX];
    char *value;
    char *end;

    value = getenv("PATH");
    if (strlen(filename) > PATH_MAX)
        fatal_error("strlen");
    while (*value != '\0')
    {
        bzero(path, PATH_MAX);
        end = strchr(value, ':');
        if (end)
            strncpy(path, value, end - value);
        else
            ft_strlcpy(path, value, PATH_MAX);
        ft_strlcat(path, "/", PATH_MAX);
        ft_strlcat(path, filename, PATH_MAX);
        if (access(path, X_OK) == 0)
        {
            char *dup;

            dup = strdup(path);
            if (dup == NULL)
                fatal_error("strdup");
            return (dup);
        }
        if (end == NULL)
            return (NULL);
        value = end + 1;
    }
    return (NULL);
}

char    **line_to_argv(char *line);
char    **args_to_argv(t_token *args);

#include <fcntl.h>

int    stashfd(int fd)
{
    int    stashfd;

    stashfd = fcntl(fd, F_DUPFD, 10);
    if (stashfd < 0)
        fatal_error("fcntl");
    if (close(fd) < 0)
        fatal_error("close");
    return (stashfd);
}

int heredoc(const char *deli)
{
    char *line;
    int pfd[2];

    if (pipe(pfd) < 0)
        return (25555);
    while (1)
    {
        line = readline("input > ");
        if (line == NULL)
            break;
        else if (strcmp(line, deli) == 0)
        {
            free(line);
            break;
        }
        dprintf(pfd[1], "%s\n", line);
        free(line);
    }
    close (pfd[1]);
    return (pfd[0]);
}

int interpret(t_command *command)
{
    extern char **environ;
    pid_t pid;
    int wstatus;
    char **argv;
    char *command_name;

    pid = fork();
    if (pid < 0)
        fatal_error("fork");
    else if (pid == 0)
    {
        argv = args_to_argv(command->args); 
        command_name = argv[0];
        execve(searchpath(command_name), argv, environ);
        fatal_error("execve\n");
        return (1);
    }
    else
    {
        wait(&wstatus);
        return (WEXITSTATUS(wstatus));
    }
}


/**
 *  line が スペースを含む時は，splitする。
 *  そうではない時は，lineとNULLからなるchar **型のデータを返す
 *
 * */
char    **line_to_argv(char *line)
{
    char    **argv;

    for (int i = 0; line[i] != '\0'; i++)
    {
        if (isspace(line[i]))
        {
            argv = ft_split(line, ' ');
            return (argv);
        }
    }
    argv = malloc(sizeof(char *) * 2);
    argv[0] = line;
    argv[1] = NULL;
    return (argv);
}

#include <stdio.h>

int abusolute_path(char *line)
{
    char    **argv;
    pid_t pid;
	int wstatus;
	extern char **environ;

    pid = fork();
    if (pid < 0)
        fatal_error("fork");
    else if (pid == 0)
    {
        argv = line_to_argv(line);
        execve(argv[0], argv, environ);
        fatal_error("execve\n");
        return (1);
    }
    else
    {
        wait(&wstatus);
        return (WEXITSTATUS(wstatus));
    }
}

void ready_redirectionfile(t_redirect *redirect)
{
    int     fd;

    while (redirect != NULL)
    {
        if (redirect->type == IN)
            fd = open(redirect->file_path, O_RDONLY);
        if (redirect->type == HEREDOC)
            fd = heredoc(redirect->file_path);
        if (redirect->type == OUT)
            fd = open(redirect->file_path, O_WRONLY | O_CREAT | O_TRUNC);
        if (redirect->type == APPEND)
            fd = open(redirect->file_path, O_CREAT | O_WRONLY | O_APPEND);
        redirect->redirectfile = fd;
        //redirect->redirectfile = stashfd(fd);
        redirect = redirect->next;
    }
}

void    redirect_reconect(t_command *command)
{
    t_redirect  *redirect;

    redirect = *(command->redirect);
    while (redirect != NULL)
    {
        if (redirect->type == IN || redirect->type == HEREDOC)
        {
            close(0);
            dup2(redirect->redirectfile, 0);
            //close(redirect->redirectfile);
            command->now_in = redirect->redirectfile;
        }
        if (redirect->type == OUT || redirect->type == APPEND)
        {
            close(1);
            dup2(redirect->redirectfile, 1);
            //close(redirect->redirectfile);
            command->now_out = redirect->redirectfile;
        }
        redirect = redirect->next;
    }
}

int    exec(t_command *command)
{
    t_redirect *f_redirect;
    pid_t pid;
    int wstatus;
    char **argv;
    char *command_name;
    extern char **environ;

    f_redirect = *(command->redirect);
    ready_redirectionfile(*(command->redirect));
    *(command->redirect) = f_redirect;
    pid = fork();
    if (pid < 0)
        fatal_error("fork");
    else if (pid == 0)
    {
        redirect_reconect(command);
	    argv = args_to_argv(command->args); 
        command_name = argv[0];
        execve(searchpath(command_name), argv, environ);
        fatal_error("execve\n");
        return (1);
    }
    else
    {
        wait(&wstatus);
        return (WEXITSTATUS(wstatus));
    }
    return (0);
}


char    **args_to_argv(t_token *args)
{
    size_t  len;
    char    **argv;

    len = 0;
    for (t_token *itr = args; itr != NULL; itr = itr->next)
        len++;
    argv = malloc(sizeof(char *) * (len + 1));
    size_t i = 0;
    for (t_token *itr = args; i != len; itr = itr->next)
    {
        argv[i] = strdup(itr->word);
        i++;
    }
    argv[len] = NULL;
    return (argv);
}
/*
void    test_args_to_argv(void)
{
    char    *line = "cat";
    t_token *t = tokenizer(line);
    t_node  *n = parse(t);
    char    **argv = args_to_argv(n->command->args);
    for (int i = 0; argv[i] != NULL; i++)
        printf("%s\n", argv[i]);
    assert(argv[1] == NULL);
    interpret(n->command);
}

int main(void)
{
    //char    *line = "/bin/ls";

    //argv = line_to_argv(line);
    //for(int i = 0; argv[i] != NULL; i++)
    //    printf("%s\n", argv[i]);
    
    //line = "echo hoge";
    //t_token *t = tokenizer(line);
    //t_node  *n = parse(t);
    //for(t_token *itr = t; itr != NULL; itr = itr->next)
    //    printf("%s\n",itr->word);

    //printf("%s\n", n->command->args->word);
    //printf("%s\n", n->command->args->next->word);

    //char **argv = args_to_argv(n->command->args);
    //for (int i = 0; argv[i] != NULL; i++)
    //    printf("%s\n", argv[i]);
    //interpret(n->command);
    //interpret(argv[0], argv);
    test_args_to_argv();
}
*/