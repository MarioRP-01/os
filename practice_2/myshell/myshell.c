#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"

// -----
// CONSTANTS
// -----
#define PROMPT_SHELL "msh"

// errors
#define ERROR_BAD_ARGUMENTS "argumento: No se admiten argumentos"
#define ERROR_INVALID_FUNCTION "mandato: No se encuentra el mandato"
#define ERROR_FILE "fichero: Error. Descripción del error"
#define ERROR_FORK "fork: Error. No se pudo crear el hijo"

// -----
// BOOL
// -----
typedef enum Bool { true = 1, false = 0 } Bool;

Bool bool(int boolean) {
  if (boolean) {
    return true;
  }
  return false;
}

// -----
// ARRAY
// -----
typedef struct Array {
  void *value;
  size_t size;
} Array;

Array array(void *value, size_t size) {
  Array array;
  array.value = value;
  array.size = size;
  return array;
}

// -----
// FUNCTION DECLARATION
// -----

// promt
Bool prompt(Array *buffer, char *cwd);

// execute commands
Bool execute_line(tline *line);

// extension parser
Bool isValidLine(tline *line);
void free_tline(tline *line);
void free_tcommand(tcommand *command);

// -----
// MAIN
// -----
int main() {
  char *cwd = getcwd(NULL, (size_t)0);

  tline *line;
  Array buffer = array(NULL, 1024);
  while (prompt(&buffer, cwd)) {
    line = tokenize(buffer.value);
    if (!isValidLine(line)) {
      fprintf(stderr, "%s\n", ERROR_INVALID_FUNCTION);
      continue;
    }
    execute_line(line);
  }

  free(cwd);
  free(buffer.value);
  free_tline(line);

  printf("\n");
  exit(0);
}

// -----
// FUNCTION IMPLEMENTATION
// -----

Bool prompt(Array *buffer, char *cwd) {
  printf("%s %s > ", PROMPT_SHELL, cwd);
  return getline((char **)&buffer->value, &buffer->size, stdin) != -1;
}

Bool execute_line(tline *line) {

  int in = 0;

  int fd[2];
  pid_t pid;

  for (int i = 0; i < line->ncommands; i++) {
    pipe(fd);

    if ((pid = fork()) < 0) {
      fprintf(stderr, "%s\n", ERROR_FORK);
    } 
    else if (pid == 0) {
      close(fd[0]);

      if (i == 0 && line->redirect_input != NULL) {
        freopen(line->redirect_input, "r", stdin);
      } else {
        dup2(in, 0);
      }

      if (i < line->ncommands - 1) {
        dup2(fd[1], 1);
      }
      else {
        if (line->redirect_output != NULL)
          freopen(line->redirect_output, "w", stdout);

        if (line->redirect_error != NULL)
          freopen(line->redirect_error, "w", stderr);
      }

      close(fd[1]);
      execv(line->commands[i].filename, line->commands[i].argv);
    } else {
      close(fd[1]);
      in = fd[0];

      if (i == line->ncommands - 1) {
        close(fd[0]);
      }
    }
    wait(NULL);
  }

  return true;
}

Bool isValidLine(tline *line) {
  Bool success = line != NULL;

  int i = 0;
  while (success && i < line->ncommands) {
    success = line->commands[i].filename != NULL;
    ++i;
  }
  return success;
}

void free_tcommand(tcommand *command) {
  if (command == NULL)
    return;
  for (int i = 0; i < command->argc; i++)
    free(command->argv[i]);
  free(command->filename);
}

void free_tline(tline *line) {
  if (line == NULL)
    return;

  if (line->ncommands == 0)
    return;

  for (int i = 0; i < line->ncommands; i++)
    free_tcommand(&line->commands[i]);
    
  free(line->commands);
  free(line->redirect_input);
  free(line->redirect_output);
  free(line->redirect_error);
}
