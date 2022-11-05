#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../test/libreria.h"

int isNatural(char * number);

int main(int argc, char *argv[])
{
  int number_of_parameters = argc - 1;
  char * function_name = argv[1];
  int N = 10;

  if (number_of_parameters < 1 || number_of_parameters > 2) {
    printf("Wrong number of parameters\n");
    return 1;
  }

  if (number_of_parameters == 2  && !isNatural(argv[number_of_parameters])) {
    printf("The second paramter must be a Natural number\n");
    return 1;
  }  

  if (number_of_parameters == 2) {
    N = atoi(argv[number_of_parameters]);
  }

  if (!strcmp(function_name, "head")) head(N); 
  else if (!strcmp(function_name, "tail")) tail(N);
  else if (!strcmp(function_name, "longlines")) longlines(N);
  else {
    printf("The first parameter must be a valid funcion (head, tail, longlines).\n");
    return 1;
  }
  return 0;
}

int isNatural(char * number) {
  int isNatural = 1;

  int i = 0;
  while (isNatural && number[i] == '\0') {
    isNatural = isdigit(number[i]);
    i++;
  }

  return isNatural;
}

