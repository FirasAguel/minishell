#include "shell.h"

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
      return (free_char_arr(dirs), NULL);
    if (access(cmd_path, X_OK) == 0)
    {
      success = 1;
      break;
    }
    free(cmd_path);
  }
  free_char_arr(dirs);
  if (!success)
    return (/*ft_puterr("Command not found.\n"),*/ NULL);
  return (cmd_path);
}

void exec_cmd(t_token *tokens, char **envp)
{
  char *cmd_path;
  char **cmd_str_split;

  cmd_str_split = build_arg_array(tokens);
  if (!cmd_str_split || !cmd_str_split[0])
  {
    ft_puterr("build_arg_array failed or empty command\n");
    exit(CMD_NOT_FOUND);
  }
  cmd_path = resolve_path((const char *)cmd_str_split[0], envp);
  if (!cmd_path)
  {
    printf("%s: command not found\n", cmd_str_split[0]);
    free_char_arr(cmd_str_split);
    exit(CMD_NOT_FOUND);
  }
  execve(cmd_path, cmd_str_split, envp);
  free_char_arr(cmd_str_split);
  perror("execve");
  free(cmd_path);
  exit(EXIT_FAILURE);
}

char *get_home_dir(char *envp[])
{
  int i;

  i = 0;
  while (envp[i] && ft_strncmp(envp[i], "HOME=", 5))
    i++;
  if (!envp[i])
    return (ft_puterr("CRITICAL: HOME not found"), NULL);
  return (strdup(envp[i] + 5));
}
