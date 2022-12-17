#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"

// -----
// ARRAY
// -----
typedef struct Array {
  void *value;
  size_t size;
} Array;

Array array(void *value, size_t size) {
  Array array;
  if (value != NULL) {
    array.value = value;
  } else {
    array.value = malloc(sizeof(char *) * size);
  }
  array.size = size;
  return array;
}

// -----
// CONSTANTS
// -----
#define PROMPT_SHELL "msh"

// errors
#define ERROR_BAD_ARGUMENTS "argumento: No se admiten argumentos."
#define ERROR_INVALID_FUNCTION "mandato: No se encuentra el mandato."
#define ERROR_FILE "fichero: Error. Descripción del error."
#define ERROR_FORK "fork: Error. No se pudo crear el hijo."
#define ERROR_BACKGROUND "bg: Error. Existen demasiados procesos en segundo plano."

#define MYSHELL_CD "cd"
#define ERROR_MANY_ARGUMENTS_CD "cd: Demasiados argumentos."
#define ERROR_EXECUTION_CD "cd: Error. No se ha podido ejecutar correctamente. %d"

#define MYSHELL_JOBS "jobs"
#define MYSHELL_UMASK "umask"
#define MYSHELL_FG "fg"
#define MYSHELL_EXIT "exit"

const char * MYSHELL_COMMANDS[] = {
  MYSHELL_CD, 
  MYSHELL_JOBS, 
  MYSHELL_UMASK, 
  MYSHELL_FG, 
  MYSHELL_EXIT
};

Array background_process;

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

typedef struct CommandRedirect {
  char *input;
  char *output;
  char *error;
} CommandRedirect;

// -----
// FUNCTION DECLARATION
// -----

// initialize
Bool initialize_myshell();

// promt
Bool prompt(Array *buffer);

// execute commands
Bool execute_line(tline *line);
Bool execute_command(
  tcommand *commands,
  int ncommands,
  CommandRedirect redirect
  );

// myshell function
Bool isMyShellCommand(char *command_name);
Bool executeMyShellCommand(Array argv);
Bool myShell_cd(Array argv);
Bool myShell_jobs(Array argv);
Bool myShell_fg(Array argv);
Bool myShell_umask(Array argv);
Bool myShell_exit(Array argv);

// extension parser
Bool isValidLine(tline *line);
Bool isValidCommand(tcommand command);
void free_tline(tline *line);
void free_tcommand(tcommand *command);

// handler

static void handler_child_end(int sig, siginfo_t *si, void *ucontext);

// -----
// MAIN
// -----
int main() {
  initialize_myshell();

  tline *line;
  Array buffer = array(NULL, 1024);
  while (prompt(&buffer)) {
    line = tokenize(buffer.value);
    if (!isValidLine(line)) {
      fprintf(stderr, "%s\n", ERROR_INVALID_FUNCTION);
      continue;
    }
    execute_line(line);
  }

  printf("Estamos saliendo :)");

  free(background_process.value);
  free(buffer.value);
  free_tline(line);

  printf("\n");
  exit(0);
}

// -----
// FUNCTION IMPLEMENTATION
// -----

// initialize
Bool initialize_myshell() {
  background_process = array(NULL, 1024);
  ((int *)background_process.value)[0] = 0;

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = handler_child_end;
  sa.sa_flags = SA_SIGINFO | SA_RESTART;

  sigaction(SIGCHLD, &sa, NULL);

  return true;
}

// prompt

Bool prompt(Array *buffer) {
  char *cwd = getcwd(NULL, (size_t)0);
  printf("%s %s > ", PROMPT_SHELL, cwd);
  Bool isSuccess = getline((char **)&buffer->value, &buffer->size, stdin) != -1;
  free(cwd);
  return isSuccess;
}

// Execute commands

Bool execute_line(tline *line) {

  CommandRedirect redirect = {
    line->redirect_input,
    line->redirect_output,
    line->redirect_error
  };

  if (!line->background) return execute_command(
    line->commands, 
    line->ncommands, 
    redirect
  );

  size_t i = 0;
  Bool has_space;
  while ((has_space = i < background_process.size)  &&
    ((int *)background_process.value)[i] != 0) {

    ++i;
  }
  if (!has_space) {
    fprintf(stderr, "%s\n", ERROR_BACKGROUND);
    return false;
  }

  pid_t pid;
  if ((pid = fork()) < 0) {
    fprintf(stderr, "%s\n", ERROR_FORK);
  }
  else if (pid == 0) {
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, 0);

    execute_command(
      line->commands,
      line->ncommands,
      redirect
    );
    _exit(0);
  }

  ((pid_t *)background_process.value)[i] = pid;

  if (++i < background_process.size)
    ((int *)background_process.value)[i] = 0;
  return true;
}

Bool execute_command(
  tcommand *commands,
  int ncommands,
  CommandRedirect redirect
) {
  int in = 0;

  int fd[2];
  pid_t pid;

  for (int i = 0; i < ncommands; i++) {
    pipe(fd);

    if ((pid = fork()) < 0) {
      fprintf(stderr, "%s\n", ERROR_FORK);
    } 
    else if (pid == 0) {
      close(fd[0]);

      if (isMyShellCommand(commands[i].argv[0])) {
        close(fd[1]);
        exit(0);
      }

      if (i == 0 && redirect.input != NULL) {
        freopen(redirect.input, "r", stdin);
      } else {
        dup2(in, 0);
      }

      if (i < ncommands - 1) {
        dup2(fd[1], 1);
      }
      else {
        if (redirect.output != NULL)
          freopen(redirect.output, "w", stdout);

        if (redirect.error != NULL)
          freopen(redirect.error, "w", stderr);
      }

      close(fd[1]);
      execv(commands[i].filename, commands[i].argv);

    } else {
      close(fd[1]);

      if (isMyShellCommand(commands[i].argv[0])) {
        Array argv = array(commands[i].argv, commands[i].argc);
        executeMyShellCommand(argv);
        in = 0;
      } else {
        in = fd[0];
      }

      if (i == ncommands - 1) {
        close(fd[0]);
      }
    }
    wait(NULL);
  }
  return true;
}

// myshell function

Bool isMyShellCommand(char *command_name) {
  int myshell_commands_size = sizeof(MYSHELL_COMMANDS) / sizeof(char *);

  int i = 0;
  Bool isValid;
  while (
    i < myshell_commands_size
    && !(isValid = strcmp(command_name, MYSHELL_COMMANDS[i]) == 0)
    ) {

    ++i;
  }
  return isValid;
}

Bool executeMyShellCommand(Array argv) {
  if (argv.size == 0) {
    return false;
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_CD) == 0) {
    return myShell_cd(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_JOBS) == 0) {
    return myShell_jobs(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_FG) == 0) {
    return myShell_fg(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_UMASK) == 0) {
    return myShell_umask(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_EXIT) == 0) {
    return myShell_exit(argv);
  }

  return false;
}

Bool myShell_cd(Array argv) {

  if (argv.size > 2) {
    fprintf(stderr, "cd: Demasiados argumentos");
    return false;
  }

  char * path;

  if (argv.size == 2) {
    path = ((char**)argv.value)[1];
  }
  else {
    path = getenv("HOME");
  }
  Bool isSuccess = chdir(path) == 0;
  
  if (!isSuccess) fprintf(stderr, ERROR_EXECUTION_CD, errno);
  
return isSuccess;
}

Bool myShell_jobs(Array argv) {
  return true;
}

Bool myShell_fg(Array argv) {
  return true;
}

Bool myShell_umask(Array argv) {
  return true;
}

Bool myShell_exit(Array argv) {
  return true;
}

// extension parser

Bool isValidLine(tline *line) {
  Bool success = line != NULL;

  int i = 0;
  while (success && i < line->ncommands) {
    success = isValidCommand(line->commands[i]);
    ++i;
  }
  return success;
}

Bool isValidCommand(tcommand command) {
  if (command.filename != NULL) {
    return true;
  }
  return isMyShellCommand(command.argv[0]);
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

static void handler_child_end(int sig, siginfo_t *si, void *ucontext) {

  size_t process_position = 0;
  Bool is_bg = false;
  while (process_position < background_process.size && 
    ((int *)background_process.value)[process_position] != 0 &&
    !(is_bg = ((int *)background_process.value)[process_position] == si->si_pid)
  ) {
    ++process_position;
  }

  if (!is_bg) {
    return;
  }

  waitpid(si->si_pid, NULL, 0);

  size_t last_insert = process_position;
  while (last_insert + 1< background_process.size && 
    ((int *)background_process.value)[last_insert + 1] != 0) {
      
    ++last_insert;
  }

  if (last_insert < background_process.size) {
    ((int *)background_process.value)[process_position] = 
      ((int *)background_process.value)[last_insert];

    ((int *)background_process.value)[last_insert] = 0;
  }
  // printf("\nEl proceso hijo ha terminado\n");
}
