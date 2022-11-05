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

  while (N > amount_printed && (read_len = getline(&line, &len, stdin) != 1)) {
    printf("%s", line);
    amount_printed++;
  }

  if (line) free(line);
  return 1;
}

int tail(int N){
  printf("tail is running\n");
  int result = 0;
  return result;
}

int longlines(int N){
  printf("longlines is running\n");
  int result = 0;
  return result;
}
