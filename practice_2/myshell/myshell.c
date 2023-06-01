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
#include <pthread.h>

#include "parser.h"

// -----
// CONSTANTS
// -----
#define PROMPT_SHELL "msh"

// errors
#define ERROR_BAD_ARGUMENTS "argumento: No se admiten argumentos."
#define ERROR_INVALID_FUNCTION "mandato: No se encuentra el mandato."
#define ERROR_MEMORY "memoria: Error. No se pudo alojar en memoria. %d"
#define ERROR_FILE "fichero: Error. Descripción del error."
#define ERROR_FORK "fork: Error. No se pudo crear el hijo."
#define ERROR_BACKGROUND "bg: Error. Existen demasiados procesos en segundo plano."

#define MYSHELL_CD "cd"
#define ERROR_MANY_ARGUMENTS_CD "cd: Demasiados argumentos."
#define ERROR_EXECUTION_CD "cd: Error. No se ha podido ejecutar correctamente. %d"
#define ERROR_PIPES_CD "cd: Error. No se admiten pipes."

#define MYSHELL_JOBS "jobs"
#define MYSHELL_UMASK "umask"

#define MYSHELL_FG "fg"
#define ERROR_WRONG_ARGUMENTS_FG "fg: Error. Numero de argumentos incorrectos. Debe mandarse solo uno."
#define ERROR_WRONG_TYPE_ARGUMENTS_FG "fg: Error. El argumento debe ser numerico y entero. %d"
#define ERROR_WRONG_INDEX_FG "fg: Error. El indice dado no existe."

#define MYSHELL_EXIT "exit"

const char * MYSHELL_COMMANDS[] = {
  MYSHELL_CD, 
  MYSHELL_JOBS, 
  MYSHELL_UMASK, 
  MYSHELL_FG, 
  MYSHELL_EXIT
};

#define PROCESS_STATE_RUNNING "Running"
#define PROCESS_STATE_STOPPED "Stopped"
#define PROCESS_STATE_DONE "Done"

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
// PROCESS DATA
// -----
typedef struct ProcessData {
  pid_t pid;
  size_t index;
  char *state;
  char *command_line;
} ProcessData;

Bool is_lower_process_by_index(ProcessData p1, ProcessData p2);
ProcessData process_data(pid_t pid, char *command_line);
void show_process_creation(ProcessData process);
void show_process_data(ProcessData process);

// -----
// PROCESSES LIST
// -----

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct OrderedList ProcessesList;

Bool insert_process_data(ProcessesList *list, ProcessData elem);
Bool remove_process_data_by_index(ProcessesList *list, size_t index);
Bool remove_process_data_by_pid(ProcessesList *list, pid_t pid);
ProcessData *get_process_by_index(ProcessesList list, size_t index);
ProcessData *get_process_by_pid(ProcessesList list, pid_t pid);
void show_processes_list(ProcessesList list);

// -----
// ORDERED LIST
// -----

typedef struct Node {
  struct ProcessData value;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct OrderedList {
  struct Node *init;
  struct Node *last;
  int length;
} OrderedList;

struct OrderedList new_ordered_list(); 
int ordered_list_length(struct OrderedList list);
int ordered_list_is_empty(struct OrderedList list);
int insert(OrderedList *list, struct ProcessData elem);
int remove_last(struct OrderedList *list);
void show_ordered_list(struct OrderedList list);
void free_ordered_list(struct OrderedList *list);
void free_node(Node node);

// -----
// CONSTANTE
// -----

ProcessesList background_process = {NULL, NULL, 0};

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
// FUNCTION DECLARATION
// -----

// initialize
Bool initialize_myshell();

// prompt
Bool prompt(Array *buffer);

// execute commands
Bool execute_line(tline *line, char *raw_line);
Bool execute_command(
  tcommand *commands,
  int ncommands,
  Bool is_background,
  CommandRedirect redirect
  );

// myshell function
Bool isMyShellCommand(char *command_name);
Bool executeMyShellCommand(Array argv);
Bool is_cd_command_with_pipes(tline *line);
Bool myShell_cd(Array argv);
Bool myShell_jobs();
Bool myShell_fg(Array argv);
Bool myShell_umask(Array argv);
Bool myShell_exit();

// extension parser
Bool isValidLine(tline *line);
Bool isValidCommand(tcommand command);
void free_tline(tline *line);
void free_tcommand(tcommand *command);

// handler

static void handler_child_end(int sig, siginfo_t *si, void *ucontext);

tline *line;
Array buffer;

// -----
// MAIN
// -----
int main() {
  initialize_myshell();

  
  buffer = array(NULL, 1024);
  while (prompt(&buffer)) {
    line = tokenize(buffer.value);
    if (!isValidLine(line)) {
      fprintf(stderr, "%s\n", ERROR_INVALID_FUNCTION);
      continue;
    }
    execute_line(line, strdup(buffer.value));
  }

  myShell_exit();
}

// -----
// FUNCTION IMPLEMENTATION
// -----

// initialize
Bool initialize_myshell() {

  signal(SIGINT, SIG_IGN);

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
  printf("%s %s >\n", PROMPT_SHELL, cwd);
  Bool isSuccess = getline((char **)&buffer->value, &buffer->size, stdin) != -1;
  free(cwd);
  return isSuccess;
}

// Execute commands

Bool execute_line(tline *line, char *raw_line) {

  if (is_cd_command_with_pipes(line)) {
    fprintf(stderr, "%s\n", ERROR_PIPES_CD);
    return false;
  }

  CommandRedirect redirect = {
    line->redirect_input,
    line->redirect_output,
    line->redirect_error
  };

  if (!line->background) return execute_command(
    line->commands, 
    line->ncommands, 
    false,
    redirect
  );

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
      true,
      redirect
    );
    _exit(0);
  }

  return insert_process_data(
    &background_process, 
    process_data(pid, raw_line)
  );
}

Bool execute_command(
  tcommand *commands,
  int ncommands,
  Bool is_background,
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
      if (!is_background) signal(SIGINT, SIG_DFL);

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
    return myShell_jobs();
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_FG) == 0) {
    return myShell_fg(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_UMASK) == 0) {
    return myShell_umask(argv);
  }

  if (strcmp(((char **)argv.value)[0], MYSHELL_EXIT) == 0) {
    return myShell_exit();
  }

  return false;
}

Bool is_cd_command_with_pipes(tline *line) {
  Bool is_cd_with_pipes = false;

  if (line->ncommands < 2) return false;

  int index = 0;
  while (index < line->ncommands && !is_cd_with_pipes) {
    is_cd_with_pipes = strcmp(
        line->commands[index].argv[0],
        MYSHELL_CD
      ) == 0;
    ++index;
  }

  return is_cd_with_pipes;
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

Bool myShell_jobs() {
  show_processes_list(background_process);
  return true;
}

Bool myShell_fg(Array argv) {
  if (argv.size != 2) {
    fprintf(stderr, "%s\n", ERROR_WRONG_ARGUMENTS_FG);
    return false;
  }

  int index = strtol(((char **)argv.value)[1], NULL, 10);
  if (errno == EINVAL) {
    fprintf(stderr, "%s\n", ERROR_WRONG_TYPE_ARGUMENTS_FG, errno);
  }
  ProcessData *process = get_process_by_index(background_process, index);
  if (process == NULL) {
    fprintf(stderr, "%s\n", ERROR_WRONG_INDEX_FG);
    return false;
  }
  
  waitpid(process->pid, NULL, 0);
  return true;
}

Bool myShell_umask(Array argv) {
  return true;
}

Bool myShell_exit() {
  free_ordered_list(&background_process);
  free(buffer.value);
  free_tline(line);

  printf("\n");
  exit(0);
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

  ProcessData *process = get_process_by_pid(background_process, si->si_pid);

  if (process == NULL) return;

  waitpid(si->si_pid, NULL, 0);

  remove_process_data_by_pid(&background_process, si->si_pid);
}

// Processes

// Processes Data

ProcessData process_data(pid_t pid,  char *command_line) {
  ProcessData process = {
    pid,
    0,
    PROCESS_STATE_RUNNING,
    command_line
  };
  return process;
}

void show_process_creation(ProcessData process) {
  printf("[%d] %d",
    (int) process.index,
    process.pid
  );
}

void show_process_data(ProcessData process) {
  printf("[%d]\t[%s]\t\t%s",
    (int) process.index,
    process.state,
    process.command_line
  );
}

Bool is_lower_process_by_index(ProcessData p1, ProcessData p2) {
  return p1.index < p2.index;
}

// Processes List

Bool insert_process_data(ProcessesList *list, ProcessData elem) {

  struct Node * node = malloc(sizeof(Node));
  node->value = elem;

  pthread_mutex_lock(&list_mutex);

  if (ordered_list_is_empty(*list)) {
    node->value.index = 1;
    list->init = node;
    list->last = node;
    list->length = 1;
    node->next = NULL;
    node->prev = NULL;

    pthread_mutex_unlock(&list_mutex);
    return true;
  }

  list->length++; 
  node->value.index = list->last->value.index + 1;

  node->next = list->last->next;
  node->prev = list->last;
  list->last->next = node;
  list->last = node;

  pthread_mutex_unlock(&list_mutex);
  return true;
}

Bool remove_process_data_by_index(ProcessesList *list, size_t index) {

  pthread_mutex_lock(&list_mutex);

  if (ordered_list_is_empty(*list)) {
    pthread_mutex_unlock(&list_mutex);
    return false;
  }

  Node *pAux = list->init;
  Bool isFound;
  while (!(isFound = pAux->value.index == index) &&
    pAux->next != NULL
  ) {
    pAux = pAux->next;
  }

  if (!isFound) {
    pthread_mutex_unlock(&list_mutex);
    return false;
  }

  if (pAux == list->last)
    list->last = pAux->prev;

  if (pAux == list->init)
    list->init = list->init->next;
  
  if (pAux->prev != NULL)
    pAux->prev->next = pAux->next;
  
  if (pAux->next != NULL)
    pAux->next->prev = pAux->prev;

  free(pAux);
  --list->length;

  pthread_mutex_unlock(&list_mutex);
  return true;
}

Bool remove_process_data_by_pid(ProcessesList *list, pid_t pid) {

  pthread_mutex_lock(&list_mutex);

  if (ordered_list_is_empty(*list)) {
    pthread_mutex_unlock(&list_mutex);
    return false;
  }

  Node *pAux = list->init;
  Bool isFound;
  while (!(isFound = pAux->value.pid == pid) &&
    pAux->next != NULL
  ) {
    pAux = pAux->next;
  }

  if (!isFound) {
    pthread_mutex_unlock(&list_mutex);
    return false;
  }

  if (pAux == list->last)
    list->last = pAux->prev;

  if (pAux == list->init)
    list->init = list->init->next;
  
  if (pAux->prev != NULL)
    pAux->prev->next = pAux->next;
  
  if (pAux->next != NULL)
    pAux->next->prev = pAux->prev;

  free(pAux);
  --list->length;

  pthread_mutex_unlock(&list_mutex);
  return true;
}

ProcessData *get_process_by_index(ProcessesList list, size_t index) {

  pthread_mutex_lock(&list_mutex);

  if (ordered_list_is_empty(list)) {
    pthread_mutex_unlock(&list_mutex);
    return NULL;
  }

  Node *pAux = list.init;
  Bool isFound;
  while (!(isFound = pAux->value.index == index) &&
    pAux->next != NULL
  ) {
    pAux = pAux->next;
  }

  if (!isFound) {
  pthread_mutex_unlock(&list_mutex);
    return NULL;
  }

  ProcessData * result = &pAux->value;
  pthread_mutex_unlock(&list_mutex);
  return result;
}

ProcessData *get_process_by_pid(ProcessesList list, pid_t pid) {

  pthread_mutex_lock(&list_mutex);

  if (ordered_list_is_empty(list)) {
    pthread_mutex_unlock(&list_mutex);
    return NULL;
  }

  Node *pAux = list.init;
  Bool isFound;
  while (!(isFound = pAux->value.pid == pid) &&
    pAux->next != NULL
  ) {
    pAux = pAux->next;
  }

  pthread_mutex_unlock(&list_mutex);
  if (!isFound) {
    pthread_mutex_unlock(&list_mutex);
    return NULL;
  }
  ProcessData * result = &pAux->value;
  pthread_mutex_unlock(&list_mutex);
  return result;
}

void show_processes_list(struct OrderedList list) {
  pthread_mutex_lock(&list_mutex);
  struct Node * pAux = list.init;
  int count = 0;
  while (pAux != NULL) {
    show_process_data(pAux->value);
    pAux = pAux->next;
    count++;
  }
  pthread_mutex_unlock(&list_mutex);
}

// Ordered List

struct OrderedList ordered_list() {
  OrderedList list = {NULL, NULL, 0};
  return list;
}

int ordered_list_is_empty(OrderedList list) {
  return (list.length == 0 || list.init == NULL || list.last == NULL);
}

int insert(OrderedList *list, ProcessData elem) {
  if (elem.command_line == NULL) return 0;
  
  struct Node * node = malloc(sizeof(Node));
  node->value = elem;

  if (ordered_list_is_empty(*list)) {
    list->init = node;
    list->last = node;
    list->length = 1;
    node->next = NULL;
    node->prev = NULL;
    return 1;
  }

  list->length++;

  struct Node * pAux = list->init;
  Bool node_is_smaller;
  while (pAux->next != NULL &&
    (node_is_smaller = is_lower_process_by_index(node->value, pAux->value))
    ) {
    pAux = pAux->next;
  }

  if (pAux->next == NULL && node_is_smaller) {
    node->next = pAux->next;
    pAux->next = node;
    node->prev = pAux;
    list->last = node;
    return 1;
  }

  if (pAux == list->init) 
    list->init = node;
  else
    pAux->prev->next = node;

  node->next = pAux;
  node->prev = pAux->prev;
  pAux->prev = node;
  
  return 1;
}

ProcessData *getLastElement(OrderedList list) {
  if (ordered_list_is_empty(list)) return NULL;

  return &list.last->value;
}

int remove_last(OrderedList * list) {
  if (ordered_list_is_empty(*list)) return 1;
  
  if (list->length == 1) list->init = NULL;

  list->length--;
  list->last = list->last->prev;

  free(list->last->next);
  list->last->next = NULL;
  return 1;
}

void free_ordered_list(OrderedList *list) {
  Node * pAux = list->init;
  while (list->init != NULL) {
    pAux = pAux->next;
    free(list->init);
    free_node(*list->init);
    list->init = pAux;
  }
  list->init = NULL;
  list->last = NULL;
}

void free_node(Node node) {
  free(node.value.command_line);
}
