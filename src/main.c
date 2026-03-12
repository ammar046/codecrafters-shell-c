#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *find_in_path(char *command)
{
  char *path_env = getenv("PATH");
  if (!path_env)
    return NULL;

  char *path_copy = strdup(path_env);
  char *dir = strtok(path_copy, ":");
  static char result_path[1024];
  int found = 0;

  while (dir != NULL)
  {
    snprintf(result_path, sizeof(result_path), "%s/%s", dir, command);
    if (access(result_path, X_OK) == 0)
    {
      found = 1;
      break; // Exit loop so we can free path_copy
    }
    dir = strtok(NULL, ":");
  }

  free(path_copy); // Always free the copy
  return found ? result_path : NULL;
}

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
    else if (strncmp(input, "echo ", 5) == 0)
    {
      printf("%s\n", input + 5);
    }
    else if (strncmp(input, "type ", 5) == 0)
    {
      if (strcmp(input + 5, "exit") == 0 || strcmp(input + 5, "echo") == 0 || strcmp(input + 5, "type") == 0)
      {
        printf("%s is a shell builtin\n", input + 5);
      }
      else
      {
        char *path_found = find_in_path(input + 5);
        if (path_found)
        {
          printf("%s is %s\n", input + 5, path_found);
        }
        else
        {
          printf("%s: not found\n", input + 5);
        }
      }
    }
    else
    {
      printf("%s: command not found\n", input);
    }

    printf("$ ");
  }

  return 0;
}
