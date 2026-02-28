#include "shell.h"

void	t_cmd_delone(t_cmd *lst)
{
	if (!lst)
		return ;
  free_char_arr(lst->argv);
	free(lst);
	return ;
}

void	t_cmd_clear(t_cmd **lst)
{
	t_cmd	*curr;
	t_cmd	*next;

	if (!lst)
		return ;
	curr = *lst;
	while (curr)
	{
		next = curr->next;
		t_cmd_delone(curr);
		curr = next;
	}
	*lst = NULL;
	return ;
}

t_cmd *generate_cmd_node(t_token *start, t_token *end, int argc)
{
  char **argv;
  t_token *ptr;
  int i;
  t_cmd *cmd;

  argv = (char **)malloc((argc + 1) * sizeof(char *));
  if (!argv)
    return (ft_puterr("generate_cmd_node: malloc fail\n"), NULL);
  cmd = (t_cmd *)malloc(sizeof(t_cmd));
  if (!cmd)
    return (ft_puterr("generate_cmd_node: malloc fail\n"), NULL);
  i = 0;
  ptr = start;  // could remove this and just use start as ptr
  while (ptr && ptr != end)
  {
    argv[i++] = strdup(ptr->value);
    ptr = ptr->next;
  }
  argv[i] = NULL;
  cmd->argv = argv;
  cmd->in = 0;
  cmd->out = 1;
  cmd->err = 2;
  cmd->next = NULL;
  return (cmd);
}

void append_cmd_node(t_cmd **head, t_cmd **tail, t_cmd *cmd_node)
{
  if (!*head)
  {
    *head = cmd_node;
    *tail = cmd_node;
  }
  else
  {
    (*tail)->next = cmd_node;
    *tail = cmd_node;
  }
}

t_cmd *parse(t_token *tokens)
{
  t_token *ptr;
  t_token *start;
  int argc;
  t_cmd *head;
  t_cmd *tail;
  t_cmd *cmd_node;

  if (tokens->type == PIPE)
    return (ft_puterr("parsing error: pipe at start\n"), NULL);
  ptr = tokens;
  start = tokens;
  argc = 0;
  head = NULL;
  tail = NULL;
  while (ptr)
  {
    if (ptr->type == PIPE)
    {
      if (!(ptr->next) || ptr->next->type == PIPE)
        return (ft_puterr("parsing error: bad pipe\n"), NULL);
      cmd_node = generate_cmd_node(start, ptr, argc);
      if (!cmd_node)
        return (ft_puterr("parsing error: cmd node generation failed\n"), NULL);
      append_cmd_node(&head, &tail, cmd_node);
      start = ptr->next;
      argc = 0;
    }
    else
      argc++;
    ptr = ptr->next;
  }
  if (argc)
  {
    cmd_node = generate_cmd_node(start, ptr, argc);
    if (!cmd_node)
      return (ft_puterr("parsing error: cmd node generation failed\n"), NULL);
    append_cmd_node(&head, &tail, cmd_node);
  }
  return (head);
}

// TODO: add export unset env
void init_builtin_cmd_arr(char ***builtin_cmds)
{
  int builtin_cmd_count;

  builtin_cmd_count = 6;
  *builtin_cmds = (char **)malloc((builtin_cmd_count + 1) * sizeof(char **));
  (*builtin_cmds)[0] = strdup("exit");
  (*builtin_cmds)[1] = strdup("echo");
  (*builtin_cmds)[2] = strdup("type");
  (*builtin_cmds)[3] = strdup("pwd");
  (*builtin_cmds)[4] = strdup("cd");
  (*builtin_cmds)[5] = strdup("history");
  (*builtin_cmds)[6] = NULL;
}

void	print_t_cmd(t_cmd *cmds)
{
	t_cmd	*ptr;
  int i;

  ptr = cmds;
	while (ptr)
	{
    printf("%s", ptr->argv[0]);
    i = 1;
    while (ptr->argv[i])
			printf(" %s", ptr->argv[i++]);
    ptr = ptr->next;
    printf("\n");
	}
}

void	close_fds(t_std_fds std_fds)
{
	close(std_fds.in);
	close(std_fds.out);
	close(std_fds.err);
}

void	rewire_fds(t_std_fds new_fds)
{
	dup2(new_fds.in, STDIN_FILENO);
	dup2(new_fds.out, STDOUT_FILENO);
	dup2(new_fds.err, STDERR_FILENO);
}
void	delegate_to_child(t_std_fds new_fds, char **argv, char **envp)
{
  rewire_fds(new_fds);
  // ft_printf_fd(2, "executing %s with fds %d %d %d\n", argv[0], new_fds.in, new_fds.out, new_fds.err);
	exec_cmd(argv, envp);
}

int handle_cmds(t_cmd *cmds, char **builtin_cmds, char **envp)
{
  t_cmd			*cmd;
  pid_t			pid;
  t_std_fds	fds;
	int				pipefd[2];
  int       next_in;

  fds = (t_std_fds){0 , 1, 2};
  cmd = cmds;
  next_in = STDIN_FILENO;
  pipefd[0] = pipefd[1] = -1;
  while (cmd)
  {
    if (cmd->next)
    {
      if (pipe(pipefd) == -1)
        return (perror("handle_cmds; pipe failed:"), 0);
      fds = (t_std_fds){next_in, pipefd[1], STDERR_FILENO};
      next_in = pipefd[0];
    }
    else
      fds = (t_std_fds){next_in, STDOUT_FILENO, STDERR_FILENO};
    pid = fork();
    if (pid < 0) // Error handling
      return (ft_puterr("handle_cmds: fork failed\n"), 0);
    else if (pid == 0)
    {
      rewire_fds(fds);
      if (handle_builtins(cmd, builtin_cmds, envp))
        exit (0);
      exec_cmd(cmd->argv, envp);
    }
    // else here is unnecessary because both branches above exit this scope
    if (fds.in != STDIN_FILENO)
      close(fds.in);
    if (fds.out != STDOUT_FILENO)
      close(fds.out);
    cmd = cmd->next;
  }
  while (waitpid(-1, NULL, 0) > 0)
    ;
  return (1);
}

int	handle_input(char **line, t_cmd	**cmds)
{
	t_token *tokens;

	// Flush after every printf
	// not required for ft_printf since it uses write and no buffer
	setbuf(stdout, NULL);
	*line = readline("$ ");
	if (!*line || !**line)
	{
		ft_puterr("input is null\n");
		return (0);
	}
	add_history(*line);
	tokens = lex(*line);
	if (!tokens)
	{
		ft_puterr("lex fail\n");
		return (0);
	}
	*cmds = parse(tokens);
	if (!*cmds)
	{
		ft_puterr("parsing fail\n");
		return (0);
	}
	ft_lstclear(&tokens);
  // print_t_cmd(parse(tokens));
  // ptr = tokens;
  // while (ptr)
  // {
  //   printf("%d\t%s\n", ptr->type, ptr->value);
  //   ptr = ptr->next;
  // }
	return (1);
}

int main(int argc, char *argv[], char *envp[])
{
  char *line;
  char **builtin_cmds;
  t_cmd		*cmds;
  int exit_code;

  // printf("path_max %d\n", PATH_MAX);
  // printf("home dir %s\n", get_home_dir(envp));

  // while (1)
  // {
  //   setbuf(stdout, NULL);
  //   line = readline(NULL);
  //   tokens = lex(line);
  //   for (size_t i = 0; tokens[i]; i++)
  //     printf("%s\n", tokens[i]);
  // }

  exit_code = EXIT_SUCCESS;
  init_builtin_cmd_arr(&builtin_cmds);
  while (1)
  {
    if (!handle_input(&line, &cmds))
      continue ;
    // print_t_cmd(cmds);
    int ret = handle_special_builtins(cmds, &exit_code, envp);
    if (!ret)
      handle_cmds(cmds, builtin_cmds, envp);
    free(line);
    t_cmd_clear(&cmds);
    if (ret == 1)
      break;
  }
  free_char_arr(builtin_cmds);
  return (exit_code);
}
