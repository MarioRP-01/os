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
void push(struct Stack stack, char * string, int length);
void show_stack();
void free_stack();


int tail(int N){
  struct Stack stack = new_stack(N);
  char * line = NULL;
  size_t len = 0;
  ssize_t read_len = 0;

  while ((read_len = getline(&line, &len, stdin) != -1)) {
    insertar(stack, line, len); 
  }

  return 1;
}

int longlines(int N){
  printf("longlines is running\n");
  int result = 0;
  return result;
}

int next(int counter, int upper_range) {
  if (counter < upper_range) {
    counter++;
  }  else {
    counter = 0;
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

int insertar(struct Stack stack, char * string, int length) {
  int init = stack.init; 
  int last = stack.last;
  int len = stack.len;

  if (string == NULL) return 0;
  
  if (stack.len == 0) return 1;

  if (stack.len == 1) stack.store[0] = string;

  if (init == last) {
    stack.store[init] = string;
    return 1;
  }

  if (
    stack.store[last] != NULL 
    && last + 1 < len 
    && stack.store[last + 1] == NULL
  ) {
    last++;
    stack.store[last] = string;

    stack.last = last;
    return 1;
  }

  /*
  if (stack.store[last] != NULL && last + 1 == len) {
    last = 0;
    stack.store[last] = string;
    
    stack.init++;
    stack.last = last;
    return 1;
  }
  */

  return 1;
}

void mostrar () {

}

void free_array() {

}
