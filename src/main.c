#include "shell.h"

void ft_puterr(char *s)
{
  write(2, s, ft_strlen(s));
}

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

int main(int argc, char *argv[], char *envp[])
{
  char *line;
  t_token *tokens;
  t_token *ptr;

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

  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    line = readline(NULL);
    if (!line)
      return (ft_puterr("input is null"), 1);

    tokens = lex(line);

    if (!tokens)
      continue;

    // t_token *ptr = tokens;
    // while (ptr)
    // {
    //   printf("%d\t%s\n", ptr->type, ptr->value);
    //   ptr = ptr->next;
    // }

    // TODO: add exit status e.g. exit 42
    if (!strcmp(tokens->value, "exit"))
      return (free(line), ft_lstclear(&tokens), 0);
    if (!strcmp(tokens->value, "echo"))
    {
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
    else if (!strcmp(tokens->value, "type"))
    {
      ptr = tokens->next;
      while (ptr)
      {
        if (!strcmp(ptr->value, "type") || !strcmp(ptr->value, "echo") || !strcmp(ptr->value, "exit") || !strcmp(ptr->value, "pwd") || !strcmp(ptr->value, "cd"))
          printf("%s is a shell builtin\n", ptr->value);
        else
        {
          char *cmd_path = resolve_path((const char *)ptr->value, envp);
          if (!cmd_path)
            printf("%s: not found\n", ptr->value);
          else
            printf("%s is %s\n", ptr->value, cmd_path);
        }
        ptr = ptr->next;
      }
    }
    else if (!strcmp(tokens->value, "pwd"))
    {
      char *buff = malloc(PATH_MAX);
      printf("%s\n", getcwd(buff, PATH_MAX));
      free(buff);
    }
    else if (!strcmp(tokens->value, "cd"))
    {
      if (!tokens->next || !strcmp(tokens->next->value, "~"))
      {
        char *home = get_home_dir(envp);
        if (!home)
          ft_puterr("home dir fail");
        else
        {
          chdir(home);
          free(home);
        }
      }
      else
      if (tokens->next && chdir(tokens->next->value))
        printf("cd: %s: No such file or directory\n", tokens->next->value);
    }
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
    // free_split(tokens);
    ft_lstclear(&tokens);
  }

  return 0;
}
