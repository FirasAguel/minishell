#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

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

  printf("%s: command not found\n", line);

  free(line);
  }

  return 0;
}
