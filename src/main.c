#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *find_in_path(char *command)
{
  char *path_env = getenv("PATH");
  if (!path_env)
    return NULL;

  char *path_copy = strdup(path_env);
  char *dir = strtok(path_copy, ":");
  static char result_path[4096];
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

void lsh_loop()
{
  printf("$ ");
  char input[100];

  while (fgets(input, sizeof(input), stdin))
  {
    input[strcspn(input, "\r\n")] = 0;
    char *cpy = malloc(strlen(input) + 1);
    if (cpy == NULL)
    {
      perror("creating a copy of input failed\n");
    }
    strcpy(cpy, input);
    char *cmd = strtok(input, " ");
    if (strcmp(cmd, "exit") == 0)
      break;
    else if (strcmp(cmd, "echo") == 0)
    {
      char *stmt = strchr(cpy, ' ');
      if (stmt != NULL)
      {
        // We found a space! Move past it.
        while (*stmt == ' ')
          stmt++;
        printf("%s\n", stmt);
      }
      else
      {
        // No space found (user just typed "echo")
        // Standard shells just print a blank line
        printf("\n");
      }
      free(cpy);
    }
    else if (strcmp(cmd, "type") == 0)
    {
      cmd = strtok(NULL, " ");
      if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "echo") == 0 || strcmp(cmd, "type") == 0)
      {
        printf("%s is a shell builtin\n", cmd);
      }
      else
      {
        char *path_found = find_in_path(cmd);
        if (path_found)
        {
          printf("%s is %s\n", cmd, path_found);
        }
        else
        {
          printf("%s: not found\n", cmd);
        }
      }
    }
    else
    {
      char *path_found = find_in_path(cmd);
      if (path_found == NULL)
      {
        printf("%s: command not found\n", cpy);
      }
      else
      {
        char *name = malloc(strlen(cmd) + 1);
        strcpy(name, cmd);
        int count = 0;
        char **args = NULL;
        args = realloc(args, (count + 1) * sizeof(char *));
        args[count] = malloc(strlen(name) + 1);
        strcpy(args[count], name);
        count++;
        char *arg = strtok(NULL, " ");
        while (arg != NULL)
        {
          args = realloc(args, (count + 1) * sizeof(char *));
          args[count] = malloc(strlen(arg) + 1);
          strcpy(args[count], arg);
          count++;
          arg = strtok(NULL, " ");
        }

        args = realloc(args, (count + 1) * sizeof(char *));
        args[count] = NULL;

        pid_t pid = fork();
        if (pid == 0)
        {
          execv(path_found, args);
          perror("exec failed!\n");
        }
        else if (pid < 0)
        {
          perror("FORK FAILED!\n");
        }
        else
        {
          wait(NULL); //  to avoid zombie process
        }
      }
    }

    printf("$ ");
  }
}

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  // main shell loop
  lsh_loop();
  return 0;
}
