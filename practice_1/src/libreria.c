#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// longlines

struct String {
  char * string;
  int length;
};

struct Node {
  struct String * value;
  struct Node * next;
  struct Node * prev;
};

struct OrderedList {
  struct Node * init;
  struct Node * last;
  int length;
};

struct String * new_string(char * string);
struct OrderedList new_ordered_list(); 
int ordered_list_length(struct OrderedList list);
int ordered_list_is_empty(struct OrderedList list);
int insert(struct OrderedList * list, struct String * elem);
int remove_last(struct OrderedList * list);
void show_ordered_list(struct OrderedList list);
void free_string(struct String * node);
void free_node(struct Node * node);
void free_ordered_list(struct OrderedList * list);

int longlines(int N){
  struct OrderedList list = new_ordered_list();
  char * line = NULL;
  size_t len = 0;

  while (getline(&line, &len, stdin) != -1) {
    insert(&list, new_string(line));
    if (ordered_list_length(list) > N) {
      remove_last(&list);
    }
    line = NULL;
    len = 0;
  }

  show_ordered_list(list);
  free_ordered_list(&list);
  
  return 1;
}


struct String * new_string(char * string) {
  struct String * elem = malloc(sizeof(struct String));
  elem->string = string;
  elem->length = strlen(string);

  return elem;
}

struct OrderedList new_ordered_list() {
  struct OrderedList list;
  list.init = NULL;
  list.last = NULL;
  list.length = 0;

  return list;
}

int ordered_list_length(struct OrderedList list) {
  return list.length;
}

int ordered_list_is_empty(struct OrderedList list) {
  return (list.length == 0 || list.init == NULL || list.last == NULL);
}

int insert(struct OrderedList * list, struct String * elem) {
  if (elem->string == NULL) return 0;
  
  struct Node * node = malloc(sizeof(struct Node));
  node->value = elem;
  node->next = NULL;
  node->prev = NULL;

  if (ordered_list_is_empty(*list)) {
    list->init = node;
    list->last = node;
    list->length = 1;
    return 1;
  }

  list->length++;

  struct Node * pPrev = list->init;
  struct Node * pNext = pPrev->next;
  int node_is_smaller = node->value->length < pPrev->value->length;

  while (pNext != NULL && node_is_smaller) {
    pPrev = pNext;
    pNext = pNext->next;
  }

  if (pPrev == list->init && !node_is_smaller) {
    node->next = pPrev;
    node->prev = NULL;
    pPrev->prev = node;
    list->init = node;
    return 1;
  }

  node->next = pNext;
  node->prev = pPrev;
  pPrev->next = node;
  
  if (pNext == NULL) list->last = node;

  return 1;
}

int remove_last(struct OrderedList * list) {
  if (ordered_list_is_empty(*list)) return 1;
  
  if (list->length == 1) list->init = NULL;

  list->length--;
  list->last = list->last->prev;

  free_node(list->last->next);
  list->last->next = NULL;
  return 1;
}

void show_ordered_list(struct OrderedList list) {
  struct Node * pAux = list.init;
  while (pAux != NULL) {
    printf("%s", pAux->value->string);
    pAux = pAux->next;
  }
}

void free_string(struct String * string) {
  free(string->string);
  free(string);
}

void free_node(struct Node * node) {
  free_string(node->value);
  free(node);
}

void free_ordered_list(struct OrderedList * list) {
  struct Node * pAux = list->init;
  while (list->init != NULL) {
    pAux = pAux->next;
    free_node(list->init);
    list->init = pAux;
  }
}
