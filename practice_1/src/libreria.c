#include <stdio.h>
#include <stdlib.h>

/*
DESCRIPCION:
Lee de la stdin e imprime las N primeras líneas recibidas.

PARAMETROS ENTRADA:
N: Números de parámetros recibidos.
VALOERES SALIDA:
0 -> Error en la ejecución
1 -> Exito en la ejecución
*/

int head(int N){
  char *line = NULL;
  size_t len = 0;
  ssize_t read_len = 0;
  int amount_printed = 0;

  while (N > amount_printed && (read_len = getline(&line, &len, stdin) != -1)) {
    printf("%s", line);
    amount_printed++;
  }

  if (line) free(line);
  return 1;
}

struct Stack {
  char ** store;
  int len;
  int init;
  int last;
};

// tail

struct Stack new_stack(int length);
int push(struct Stack * stack, char * string, int length);
int show_stack(struct Stack stack);
void free_stack(struct Stack * stack);


int tail(int N){
  struct Stack stack = new_stack(N);
  char * line = NULL;
  size_t len = 0;
  ssize_t read_len = 0;

  while ((read_len = getline(&line, &len, stdin) != -1)) {
    push(&stack, line, len); 
    line = NULL;
    len = 0;
  }
  show_stack(stack);
  free_stack(&stack); 
  return 1; 
}

int next(int counter, int length) {
  if (counter < length - 1) {
    counter++;
  }  else {
    counter = 0;
  }
  return counter;
}

int previous(int counter, int length) {
  if (counter > 0) {
    counter--;
  } else {
    counter = length - 1;
  }
  return counter;
}

struct Stack new_stack(int length) {
  
  struct Stack stack;
  char ** store = malloc(length * sizeof(char *));
  stack.store = store;
  
  for (int i = 0; i < length; i++) {
    stack.store[i] = NULL;
  }

  stack.len = length;
  stack.init = 0;
  stack.last = 0;
  
  return stack;
}

int push(struct Stack * stack, char * string, int length) {
  if (string == NULL) return 0;

  if (stack->len == 0) return 1;

  if (stack->store[stack->init] == NULL) {
    stack->store[stack->init] = string;
    return 1;
  }

  if (stack->init == stack->last) {
    stack->last = next(stack->last, stack->len);
    stack->store[stack->last] = string;
    return 1;
  }

  stack->last = next(stack->last, stack->len);

  if (stack->store[stack->last] == NULL) {
    stack->store[stack->last] = string;
    return 1;
  }

  free(stack->store[stack->init]);
  stack->init = next(stack->init, stack->len);

  stack->store[stack->last] = string;
  return 1;
}

int show_stack (struct Stack stack) {
  int init = stack.init;
  int last = stack.last;
  int length = stack.len;

  if (stack.store[init] == NULL) {
    return 0;
  }

  while (init != last) {
    printf("%s", stack.store[init]);
    init = next(init, length);
  };

  printf("%s", stack.store[init]);

  return 1;
}

void free_stack(struct Stack * stack) { 
  int length = stack->len;
  for (int i = 0; i < length; i++) {
    if (stack->store[i] != NULL) {
      free(stack->store[i]);
    } 
  }

  free(stack->store);

  stack->len = 0;
  stack->init = 0;
  stack->last = 0;
  stack->store = NULL;
}

int longlines(int N){
  printf("longlines is running\n");
  int result = 0;
  return result;
}
