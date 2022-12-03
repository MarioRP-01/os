#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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
// REDIRECTIONS
// -----
typedef struct Redirections {
  FILE *stdin;
  FILE *stdout;
  FILE *stderr;
} Redirections;

Redirections redirections(FILE *stdin, FILE *stdout, FILE *stderr) {
  Redirections redirections;
  redirections.stdin = stdin;
  redirections.stdout = stdout;
  redirections.stderr = stderr;
  return redirections;
}

// -----
// FUNCTION DECLARATION
// -----

// promt
Bool prompt(Array *buffer, char *cwd);

// execute commands
void execute_line(tline);
void execute_command(tcommand command, Redirections redirections);

// extension parser
Bool isValidLine(tline *line);
void free_tline(tline *line);
void free_tcommand(tcommand *command);

// -----
// MAIN
// -----
int main(int argc, char **argv) {

  if (argc != 1) {
    fprintf(stderr, "%s\n", ERROR_BAD_ARGUMENTS);
    exit(-1);
  }

  char *cwd = getcwd(NULL, (size_t)0);

  tline *line;
  Array buffer = array(NULL, 1024);
  while (prompt(&buffer, cwd)) {
    line = tokenize(buffer.value);
    if (!isValidLine(line)) {
      fprintf(stderr, "%s\n", ERROR_INVALID_FUNCTION);
      continue;
    }
  }

  free(cwd);
  free(buffer.value);
  free_tline(line);
  exit(0);
}

// -----
// FUNCTION IMPLEMENTATION
// -----

Bool prompt(Array *buffer, char *cwd) {
  printf("%s %s > ", PROMPT_SHELL, cwd);
  return getline((char **)&buffer->value, &buffer->size, stdin) != -1;
}

void execute_line(tline line) {}

void execute_command(tcommand command, Redirections redirections) {
  pid_t pid = fork();
  int status;
  if (pid < 0) {
    fprintf(stderr, "%s\n", ERROR_FORK);
    exit(-2);
  } else if (pid == 0) {
    execv(command.filename, command.argv);
  } else {
    wait(&status);
  }
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
  for (int i = 0; i < line->ncommands; i++)
    free_tcommand(&line->commands[i]);
  free(line->commands);
  free(line->redirect_input);
  free(line->redirect_output);
  free(line->redirect_error);
}
