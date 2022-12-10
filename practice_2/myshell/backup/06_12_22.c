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
// REDIRECTIONS
// -----
typedef enum FileDefinitionType {
  file_descriptor = 0,
  file_route = 1,
  file_default = 2,
} FileDefinitionType;

typedef struct FileDefinition {
  void *value;
  FileDefinitionType type;
} FileDefinition;

typedef struct Redirections {
  FileDefinition stdin;
  FileDefinition stdout;
  FileDefinition stderr;
} Redirections;

Redirections redirections(FileDefinition in, FileDefinition out,
                          FileDefinition err) {
  Redirections redirections;
  redirections.stdin = in;
  redirections.stdout = out;
  redirections.stderr = err;
  return redirections;
}

// -----
// FUNCTION DECLARATION
// -----

// promt
Bool prompt(Array *buffer, char *cwd);

// execute commands
Bool execute_line(tline *line);
Bool execute_command(tcommand command, Redirections redirections);

// extension file_definition
Bool redirect(FileDefinition redir, char *mode, FILE *file);

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
    execute_line(line);
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

Bool execute_line(tline *line) {
  Redirections redir;

  for (int i = 0; i < line->ncommands; i++) {
    if (i == 0) {
      if ((redir.stdin.value = line->redirect_input) == NULL)
        redir.stdin.type = file_default;
      else
        redir.stdin.type = file_route;
    } else {
      // TODO: Redirect stdin from stdout of previous command
    }
    if (i == line->ncommands - 1) {
      if ((redir.stdout.value = line->redirect_output) == NULL)
        redir.stdout.type = file_default;
      else
        redir.stdout.type = file_route;

      if ((redir.stderr.value = line->redirect_error) == NULL)
        redir.stderr.type = file_default;
      else
        redir.stderr.type = file_route;
    } else {
      // TODO: Redirect stdout to next command
    }
    return execute_command(line->commands[i], redir);
  }
  return true;
}

Bool execute_command(tcommand command, Redirections redir) {
  int fd[2];
  pid_t pid = fork();

  int status;
  if (pid < 0) {
    fprintf(stderr, "%s\n", ERROR_FORK);
    exit(-2);
  } else if (pid == 0) {
    redirect(redir.stdin, "r", stdin);
    redirect(redir.stdout, "w", stdout);
    redirect(redir.stderr, "w", stderr);
    execv(command.filename, command.argv);
    printf("continua");
  } else {
    wait(&status);
  }
  return true;
}

Bool redirect(FileDefinition redir, char *mode, FILE *file) {
  if (redir.type == file_route) {
    if (freopen(redir.value, mode, file) == NULL)
      fprintf(stderr, "%s. %s\n", ERROR_FILE, strerror(errno));
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
  for (int i = 0; i < line->ncommands; i++)
    free_tcommand(&line->commands[i]);
  free(line->commands);
  free(line->redirect_input);
  free(line->redirect_output);
  free(line->redirect_error);
}
