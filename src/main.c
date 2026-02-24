#include "shell.h"

void ft_puterr(char *s)
{
  write(2, s, ft_strlen(s));
}

char *ft_strjoin_path(char const *dir, char const *cmd)
{
  int l1;
  int l2;
  char *str;

  if (!dir || !cmd)
    return (NULL);
  l1 = ft_strlen(dir);
  l2 = ft_strlen(cmd);
  str = (char *)malloc(l1 + l2 + 2);
  if (str == NULL)
    return (NULL);
  ft_strncpy(str, dir, l1);
  str[l1] = PATH_SEP;
  ft_strncpy(str + l1 + 1, cmd, l2);
  str[l1 + 1 + l2] = '\0';
  return (str);
}

// char	*handle_relative_path(const char *cmd)
// {
// 	char	*cmd_escaped_str;

// 	cmd_escaped_str = ft_strndup(cmd, ft_strlen(cmd));
// 	// ft_printf("%s\t%s\n", cmd, cmd_escaped_str);
// 	if (access(cmd_escaped_str, F_OK) != 0)
// 		return (perror(cmd_escaped_str), free(cmd_escaped_str), NULL);	// No such file
// 	else if (access(cmd_escaped_str, X_OK) != 0)
// 		return (perror(cmd), free(cmd_escaped_str), NULL);
// 	return (cmd_escaped_str);
// }

char **get_path_split_arr(char *envp[])
{
  int i;
  char **dirs;

  i = 0;
  while (envp[i] && ft_strncmp(envp[i], "PATH=", 5))
    i++;
  if (!envp[i])
    return (ft_puterr("CRITICAL: PATH not found"), NULL);
  dirs = ft_split(envp[i] + 5, ENV_PATH_SEP);
  if (!dirs)
    return (ft_puterr("malloc failed\n"), NULL);
  return (dirs);
}

char *resolve_path(const char *cmd, char *envp[])
{
  int i;
  int success;
  char **dirs;
  char *cmd_path;

  if (!cmd || !cmd[0])
    return (/*ft_puterr("NULL or empty cmd str.\n"),*/ NULL);
  // if (ft_strchr(cmd, PATH_SEP))
  // 	return (handle_relative_path(cmd));
  dirs = get_path_split_arr(envp);
  success = 0;
  i = -1;
  while (dirs[++i] && dirs[i][0] && !success)
  {
    cmd_path = ft_strjoin_path(dirs[i], cmd);
    if (!cmd_path)
      return (free_split(dirs), NULL);
    if (access(cmd_path, X_OK) == 0)
    {
      success = 1;
      break;
    }
    free(cmd_path);
  }
  free_split(dirs);
  if (!success)
    return (/*ft_puterr("Command not found.\n"),*/ NULL);
  return (cmd_path);
}

void exec_cmd(char *cmd_str, char **envp)
{
  char **cmd_str_split;
  char *cmd_path;

  cmd_str_split = ft_split(cmd_str, ' ');
  if (!cmd_str_split || !cmd_str_split[0])
  {
    ft_puterr("empty command\n");
    exit(CMD_NOT_FOUND);
  }
  cmd_path = resolve_path((const char *)cmd_str_split[0], envp);
  if (!cmd_path)
  {
    printf("%s: command not found\n", cmd_str_split[0]);
    free_split(cmd_str_split);
    exit(CMD_NOT_FOUND);
  }
  execve(cmd_path, cmd_str_split, envp);
  free_split(cmd_str_split);
  perror("execve failed:");
  free(cmd_path);
  exit(EXIT_FAILURE);
}
#include<limits.h>
int main(int argc, char *argv[], char *envp[])
{
  char *line;

  printf("path_max %d\n", PATH_MAX);
  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    line = readline(NULL);

    if (!strcmp(line, "exit"))
      return (free(line), 0);

    if (!ft_strncmp(line, "echo", 4))
    {
      char **arr = ft_split(line, ' ');
      // printf("%s\n", line + 4);
      if (arr[1])
      {
        printf("%s", arr[1]);
        for (int i = 2; arr[i]; i++)
          printf(" %s", arr[i]);
      }
      printf("\n");
    }
    else if (!ft_strncmp(line, "type", 4))
    {
      char **arr = ft_split(line, ' ');
      for (int i = 1; arr[i]; i++)
      {
        if (!strcmp(arr[i], "type") || !strcmp(arr[i], "echo") || !strcmp(arr[i], "exit"))
          printf("%s is a shell builtin\n", arr[i]);
        else
        {
          char *cmd_path = resolve_path((const char *)arr[i], envp);
          if (!cmd_path)
            printf("%s: not found\n", arr[i]);
          else
            printf("%s is %s\n", arr[i], cmd_path);
        }
      }
    }
    else if (!ft_strncmp(line, "pwd", 3))
    {
      char *buff = malloc(PATH_MAX);
      printf("%s\n", getcwd(buff, PATH_MAX));
      free(buff);
    }
    else
    {
      pid_t pid = fork();
      if (pid < 0) // Error handling
        return (/*ft_puterr("Fork 1) failed\n"),*/ EXIT_FAILURE);
      else if (pid == 0)
        exec_cmd(line, envp);
      else
        waitpid(pid, NULL, 0); // parent waits for child to finish
    }
    free(line);
  }

  return 0;
}
