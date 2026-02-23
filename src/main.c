#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

char	**ft_split(char const *s, char c);

int main(int argc, char *argv[])
{
  char *line;

  while (1)
  {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  line = readline(NULL);

  if (!strcmp(line, "exit"))
    return (free(line), 0);

  char buff[5];
  memcpy(buff, line, 4);
  buff[4] = '\0';
  if (!strcmp(buff, "echo"))
  {
    char **arr = ft_split(line, ' ');
    //printf("%s\n", line + 4);
    if (arr[1])
    {
      printf("%s", arr[1]);
      for(int i = 2; arr[i]; i++)
        printf(" %s", arr[i]);
    }
    printf("\n");
  }
  else if (!strcmp(buff, "type"))
  {
    char **arr = ft_split(line, ' ');
    for(int i = 1; arr[i]; i++)
    {
      if (!strcmp(arr[i], "type") || !strcmp(arr[i], "echo") || !strcmp(arr[i], "exit"))
        printf("%s is a builtin\n", arr[i]);
      else 
        printf("type: Could not find '%s'\n", arr[i]);
    }
  }
  else 
  printf("%s: command not found\n", line);

  free(line);
  }

  return 0;
}
