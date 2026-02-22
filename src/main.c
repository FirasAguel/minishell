#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  char *line = readline(NULL);

  printf("%s: command not found\n", line);

  free(line);

  return 0;
}
