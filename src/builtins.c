#include "shell.h"

int is_special_builtin(char *cmd_name)
{
	return (!strcmp(cmd_name, "exit") || !strcmp(cmd_name, "cd")
		|| !strcmp(cmd_name, "export") || !strcmp(cmd_name, "unset"));
}

// TODO: add export and unset
int	handle_special_builtins(t_cmd *cmds, int *exit_code, char **envp)
{
	if (cmds->next != NULL || !is_special_builtin(cmds->argv[0]))
		return (0);
	if (!strcmp(cmds->argv[0], "exit"))
		return (handle_exit(cmds->argv, exit_code));
	if (!strcmp(cmds->argv[0], "cd"))
		return (handle_cd(cmds->argv, envp), 2);
	return (0);
}

// handle builtins other than exit
int	handle_builtins(t_cmd *cmds, char *builtin_cmds[], char **envp)
{
	if (!strcmp(cmds->argv[0], "echo"))
      return (handle_echo(cmds), 1);
    if (!strcmp(cmds->argv[0], "type"))
      return (handle_type(cmds, builtin_cmds, envp), 1);
    if (!strcmp(cmds->argv[0], "pwd"))
      return (handle_pwd(), 1);
    if (!strcmp(cmds->argv[0], "exit"))
      return (1);
    if (!strcmp(cmds->argv[0], "cd"))
      return (1);
    // not required and not allowed for minishell
    if (!strcmp(cmds->argv[0], "history"))
      return (handle_history(cmds), 1);
	return (0);
}

int	handle_exit(char **argv, int *exit_code)
{
	if (!argv[1])
	{
		*exit_code = EXIT_SUCCESS;
		return (1);
	}
	if (argv[2])
		return (ft_puterr("exit: too many arguments\n"), 0);
	if (is_number(argv[1]))
	{
		*exit_code = atoi(argv[1]) % 256;
		return (1);
	}
	else
		return (ft_puterr("exit: numeric argument is required\n"), 0);
}

// TODO: option -n
void	handle_echo(t_cmd *cmds)
{
	int	i;

	if (cmds->argv[1])
	{
		printf("%s", cmds->argv[1]);
		i = 2;
		while (cmds->argv[i])
			printf(" %s", cmds->argv[i++]);
	}
	printf("\n");
}

void	handle_cd(char **argv, char **envp)
{
	char	*home;

	if (!argv[1] || !strcmp(argv[1], "~"))
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
	else if (argv[1] && chdir(argv[1]))
		printf("cd: %s: No such file or directory\n", argv[1]);
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

void	handle_type(t_cmd *cmds, char *builtin_cmds[], char **envp)
{
	char *cmd_path;
	int	i;

	i = 1;
	while (cmds->argv[i])
	{
		if (str_in_arr(cmds->argv[i], builtin_cmds))
			printf("%s is a shell builtin\n", cmds->argv[i]);
		else
		{
			cmd_path = resolve_path((const char *)cmds->argv[i], envp);
			if (!cmd_path)
				printf("%s: not found\n", cmds->argv[i]);
			else
				printf("%s is %s\n", cmds->argv[i], cmd_path);
			free(cmd_path);
		}
		i++;
	}
}

int	is_positive_number(const char *nptr)
{
	return (nptr[0] != '-' && is_number(nptr));
}

// // not required and not allowed for minishell
void	handle_history(t_cmd *cmds)
{
	HISTORY_STATE *history_state;
	HIST_ENTRY **history_entries;
	int i = 0;
	int line_count;

	history_state = history_get_history_state();
	history_entries = history_list();
	if (cmds->argv[1])
	{
		if (cmds->argv[2])
			return (ft_puterr("history: too many arguments\n"));
		if(is_positive_number(cmds->argv[1]))
		{
			line_count = atoi(cmds->argv[1]);
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
