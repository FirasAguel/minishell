#include "shell.h"

void ft_puterr(char *s)
{
  write(2, s, ft_strlen(s));
}

int main(int argc, char *argv[], char *envp[])
{
  char *line;
  char **tokens;

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
    if (!tokens || !tokens[0])
      continue;

    // TODO: add exit status e.g. exit 42
    if (!strcmp(tokens[0], "exit"))
      return (free(line), 0);

    if (!ft_strncmp(tokens[0], "echo", 4))
    {
      if (tokens[1])
      {
        printf("%s", tokens[1]);
        for (int i = 2; tokens[i]; i++)
          printf(" %s", tokens[i]);
      }
      printf("\n");
    }
    else if (!strcmp(tokens[0], "type"))
    {
      for (int i = 1; tokens[i]; i++)
      {
        if (!strcmp(tokens[i], "type") || !strcmp(tokens[i], "echo") || !strcmp(tokens[i], "exit") || !strcmp(tokens[i], "pwd") || !strcmp(tokens[i], "cd"))
          printf("%s is a shell builtin\n", tokens[i]);
        else
        {
          char *cmd_path = resolve_path((const char *)tokens[i], envp);
          if (!cmd_path)
            printf("%s: not found\n", tokens[i]);
          else
            printf("%s is %s\n", tokens[i], cmd_path);
        }
      }
    }
    else if (!strcmp(tokens[0], "pwd"))
    {
      char *buff = malloc(PATH_MAX);
      printf("%s\n", getcwd(buff, PATH_MAX));
      free(buff);
    }
    else if (!strcmp(tokens[0], "cd"))
    {
      if (!tokens[1] || !strcmp(tokens[1], "~"))
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
      if (chdir(tokens[1]))
        printf("cd: %s: No such file or directory\n", tokens[1]);
    }
    else if (tokens && tokens[0])
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
    free_split(tokens);
  }

  return 0;
}
