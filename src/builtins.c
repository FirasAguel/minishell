#include "shell.h"

// typedef struct s_builtin
// {
//   char* cmd;
//   void (*f)(int, int);
// }	t_builtin;

// int	handle_builtins(char **tokens, char **envp)
// {
// 	const t_builtin cmds[10];
// 	...
// }

int	handle_exit(t_token **tokens, int *exit_code, char **line)
{
	if (strcmp((*tokens)->value, "exit"))
		return (0);
	if (!(*tokens)->next)
	{
		free(*line);
		ft_lstclear(tokens);
		*exit_code = 0;
		return (1);
	}
	if ((*tokens)->next->next)
		return (ft_puterr("exit: too many arguments\n"), 0);
	if (is_number((*tokens)->next->value))
	{
		free(*line);
		*exit_code = atoi((*tokens)->next->value) % 256;
		ft_lstclear(tokens);
		return (1);
	}
	else
		return (ft_puterr("exit: numeric argument is required\n"), 0);
}

void	handle_echo(t_token *tokens)
{
	t_token	*ptr;

	if (tokens->next && tokens->next->value)
	{
		printf("%s", tokens->next->value);
		ptr = tokens->next->next;
		while (ptr)
		{
			printf(" %s", ptr->value);
			ptr = ptr->next;
		}
	}
	printf("\n");
}

void	handle_cd(t_token *tokens, char **envp)
{
	char	*home;

	if (!tokens->next || !strcmp(tokens->next->value, "~"))
	{
		home = get_home_dir(envp);
		if (!home)
			ft_puterr("get_home_dir fail");
		else
		{
			chdir(home);
			free(home);
		}
	}
	else if (tokens->next && chdir(tokens->next->value))
		printf("cd: %s: No such file or directory\n", tokens->next->value);
}

void	handle_pwd()
{
	char	*buff;
	char	*pwd;

	buff = malloc(PATH_MAX);
	if (!buff)
	{
		ft_puterr("handle_pwd: malloc fail");
		return ;
	}
	pwd = getcwd(buff, PATH_MAX);
	if (!pwd)
	{
		ft_puterr("handle_pwd: getcwd fail");
		return ;
	}
	printf("%s\n", pwd);
	free(buff);
}

void	handle_type(t_token *tokens, char *builtin_cmds[], char **envp)
{
	t_token	*ptr;
	char *cmd_path;


	ptr = tokens->next;
	while (ptr)
	{
		if (str_in_arr(ptr->value, builtin_cmds))
			printf("%s is a shell builtin\n", ptr->value);
		else
		{
			cmd_path = resolve_path((const char *)ptr->value, envp);
			if (!cmd_path)
				printf("%s: not found\n", ptr->value);
			else
				printf("%s is %s\n", ptr->value, cmd_path);
		}
		ptr = ptr->next;
	}
}

int	is_number(const char *nptr)
{
	int	l;
	int	digits;

	if (!nptr || !*nptr)
		return (0);
	l = 0;
	if (nptr[l] && nptr[l] == '+')
		l++;
	digits = 0;
	while (nptr[l] && nptr[l] >= '0' && nptr[l] <= '9' && ++digits)
		l++;
	if (nptr[l] == '\0' && digits > 0)
		return (l);
	else
		return (0);
}

int	is_positive_number(const char *nptr)
{
	return (nptr[0] != '-' && is_number(nptr));
}

// not required and not allowd for minishell
void	handle_history(t_token *tokens)
{
	HISTORY_STATE *history_state;
	HIST_ENTRY **history_entries;
	int i = 0;
	int line_count;

	history_state = history_get_history_state();
	history_entries = history_list();
	if (tokens->next)
	{
		if(is_positive_number(tokens->next->value))
		{
			line_count = atoi(tokens->next->value);
			i = history_state->length - line_count;
		}
		else
			return (ft_puterr("history: invalid input\n"));
	}
	if (history_entries && history_state && history_state->length > 0)
		for (; i < history_state->length; i++)
			printf("%4d  %s\n", i + 1, history_entries[i]->line);
	else
		printf(" History list is empty.\n");
}
