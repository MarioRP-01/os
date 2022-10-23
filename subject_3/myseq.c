#include <stdio.h>

typedef enum ResponseCode 
{
  Ok = 1,
  Error = 0
} ResponseCode;

typedef struct Response 
{
  enum ResponseCode response_code;
  void *value;
} Response;

struct Response check_parameters(int argc, char *argv[])
{
  int count = 0;
  for (int i = 1; i <= argc; i++) {
    printf("%d \n", i);
  }
}

int main(int argc, char* argv[])
{
  check_parameters(argc, argv);
  return 0;
}
