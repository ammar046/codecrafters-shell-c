#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");
  char input[100];

  while (fgets(input, sizeof(input), stdin))
  {
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input, "exit") == 0)
      break;
    printf("%s: command not found\n", input);
    printf("$ ");
  }

  return 0;
}
