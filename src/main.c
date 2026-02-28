#include "shell.h"

void ft_puterr(char *s)
{
  write(2, s, ft_strlen(s));
}

// TODO: pre-build for each command during parsing instead
char  **build_arg_array(t_token *tokens)
{
  int count;
  int i;
  char **args;
  t_token *ptr;

  count = ft_lstsize(tokens);
  args = (char **)malloc((count + 1) * sizeof(char *));
  if (!args)
    return (ft_puterr("build_arg_array malloc fail\n"), NULL);
  ptr = tokens;
  i = 0;
  while (ptr)
  {
    args[i++] = strdup(ptr->value);
    ptr = ptr->next;
  }
  args[i++] = NULL;
  return (args);
}

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

int main(int argc, char *argv[], char *envp[])
{
  char *line;
  t_token *tokens;
  t_token *ptr;
  char **builtin_cmds;
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
    // Flush after every printf
    setbuf(stdout, NULL);

    line = readline("$ ");
    if (!line || !*line)
      continue;
      // return (ft_puterr("input is null\n"), 1);
    add_history(line);

    tokens = lex(line);
    if (!tokens)
    {
      ft_puterr("lex fail: no tokens\n");
      continue;
    }

    // ptr = tokens;
    // while (ptr)
    // {
    //   printf("%d\t%s\n", ptr->type, ptr->value);
    //   ptr = ptr->next;
    // }

    // TODO: implement handle_builtins instead of the if(!strcmp) else tree
    if (handle_exit(&tokens, &exit_code, &line))
      break ;
    else if (!strcmp(tokens->value, "echo"))
      handle_echo(tokens);
    else if (!strcmp(tokens->value, "type"))
      handle_type(tokens, builtin_cmds, envp);
    else if (!strcmp(tokens->value, "pwd"))
      handle_pwd();
    else if (!strcmp(tokens->value, "cd"))
      handle_cd(tokens, envp);
    // not required and not allowd for minishell
    else if (!strcmp(tokens->value, "history"))
      handle_history(tokens);
    else if (tokens && tokens->value)
    {
      pid_t pid = fork();
      if (pid < 0) // Error handling
        return (/*ft_puterr("Fork 1) failed\n"),*/ EXIT_FAILURE);
      else if (pid == 0)
        exec_cmd(tokens, envp);
      else
        waitpid(pid, NULL, 0); // parent waits for child to finish
    }
    free(line);
    ft_lstclear(&tokens);
  }
  free_char_arr(builtin_cmds);
  return (exit_code);
}
