#include "libreria.h"

int main(int argc, char *argv[])
{
  int number_of_parameters = argc - 1;

  // TODO: Parameter chek
  if (number_of_parameters < 1) {
    printf("Wrong number of parameters\n");
    return 1;
  }

  // TODO: Redirect to librarie's function
  printf("It works\n");
  return 0;
}
