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

struct Stack new_stack(int length);
int push(struct Stack stack, char * string, int length);
int show_stack(struct Stack stack);
void free_stack();


int tail(int N){
  struct Stack stack = new_stack(N);
  char * line = NULL;
  size_t len = 0;
  ssize_t read_len = 0;

  while ((read_len = getline(&line, &len, stdin) != -1)) {
    push(stack, line, len); 
  }

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
  char * store[length];
  stack.store = store;
  
  for (int i = 0; i < length; i++) {
    stack.store[i] = NULL;
  }

  stack.len = length;
  stack.init = 0;
  stack.last = 0;
  
  return stack;
}

int push(struct Stack stack, char * string, int length) {
  int init = stack.init; 
  int last = stack.last;

  if (string == NULL) return 0;
  
  if (stack.len == 0) return 1;

  if (init == last) {
    stack.store[init] = string;
    return 1;
  }

  last = next(last, length);

  if (stack.store[last] == NULL) {
    stack.store[last] = string;

    stack.last = last;
    return 1;
  }

  free(stack.store[init]);
  init = next(init, length);

  stack.last = last;
  stack.init = init;
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
    printf("%s\n", stack.store[init]);
    init = next(init, length);
  };

  printf("%s\n", stack.store[init]);

  return 1;
}

void free_stack(struct Stack stack) { 
  int length = stack.len;

  for (int i = 0; i < length; i++) {
    if (stack.store[i] != NULL) {
      free(stack.store[i]);
    } 
  }
  free(stack.store);
}

int longlines(int N){
  printf("longlines is running\n");
  int result = 0;
  return result;
}
