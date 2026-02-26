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

void exec_cmd(char **cmd_str_split, char **envp)
{
  // char **cmd_str_split;
  char *cmd_path;

  // cmd_str_split = ft_split(cmd_str, ' ');
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

enum e_lexing_modes
{
  MODE_NORMAL,
  MODE_SINGLE_QUOTE,
  MODE_DOUBLE_QUOTE
};

int update_mode(char c, enum e_lexing_modes *mode)
{
  if (!c)
    return (0);
  if (c == '\'' && *mode != MODE_DOUBLE_QUOTE)
  {
    if (*mode == MODE_SINGLE_QUOTE)
      *mode = MODE_NORMAL;
    else
      *mode = MODE_SINGLE_QUOTE;
    return (1);
  }
  if (c == '"' && *mode != MODE_SINGLE_QUOTE)
  {
    if (*mode == MODE_DOUBLE_QUOTE)
      *mode = MODE_NORMAL;
    else
      *mode = MODE_DOUBLE_QUOTE;
    return (1);
  }
  return (0);
}

// int should_append_to_token(char c, enum e_lexing_modes mode)
// {
//   if (mode == MODE_SINGLE_QUOTE) // single quote chars shouldn't reach this func call
//     return (1);
//   if (mode == MODE_NORMAL)
//   {
//     if (c == ' ') // TODO: || c == '"' || c == '|')
//       return (0);
//     else
//       return (1);
//   }
// }

int is_delimiter(char c, enum e_lexing_modes mode)
{
  if (mode == MODE_NORMAL)
    if (c == ' ') // TODO: || c == '"' || c == '|' || c == '|')
      return (1);
  return (0);
}

void flush_token(char buff[1024], int *buff_i, char ***tokens, int *token_count)
{
  buff[*buff_i] = '\0';
  (*tokens)[(*token_count)++] = strdup(buff);
  *buff_i = 0;
}

// typedef struct s_lexer
// {
//   enum e_lexing_modes mode;
// }	t_lexer;


char **lex(char *line)
{
  enum e_lexing_modes mode;
  int i;

  // TODO: use a linked list instead
  char **tokens = (char **)malloc(100 * sizeof(char *));
  int token_count;

  // TODO use append or ft_realloc
  char buff[1024];
  int buff_i;

  int token_started;

  token_count = 0;
  i = 0;
  mode = MODE_NORMAL;
  buff_i = 0;

  while (line[i])
  {
    if (update_mode(line[i], &mode))
      token_started = 1;
    else if (!is_delimiter(line[i], mode))
    {
      token_started = 1;
      buff[buff_i++] = line[i]; // append_to_token
    }
    else if (token_started)
    {
      flush_token(buff, &buff_i, &tokens, &token_count);
      token_started = 0;
    }
    i++;
  }
  if (mode != MODE_NORMAL)
    ft_puterr("syntax error (unclosed quote)\n");
  if (token_started)
    flush_token(buff, &buff_i, &tokens, &token_count);
  tokens[token_count] = NULL;
  return (tokens);
}
// i = skip_space(line, i);        // same as if (line[i] && line[i] == ' '){i++;continue;}

#include <limits.h>
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

  // TODO: replace strncmps with split[0] once tokenization is implemented
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

    // using strncmp is bad because it ignores what comes directly after the command. e.g. echonxyz arg1 arg2 will behave as echo arg1 arg2
    if (!ft_strncmp(tokens[0], "echo", 4))
    {
      // char **arr = ft_split(line, ' ');
      // printf("%s\n", line + 4);
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
      // char *buff = malloc(PATH_MAX);
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
