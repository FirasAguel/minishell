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

// not required and not allowd for minishell
void	handle_history()
{
	HISTORY_STATE *history_state;
	HIST_ENTRY **history_entries;

	history_state = history_get_history_state();
	history_entries = history_list();

	if (history_entries && history_state && history_state->length > 0)
		for (int i = 0; i < history_state->length; i++)
			printf("%4d  %s\n", i + 1, history_entries[i]->line);
	else
		printf(" History list is empty.\n");
}
