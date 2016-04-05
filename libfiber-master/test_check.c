#include <stdio.h>
#include "include/fiber_manager.h"
#include <string.h>

void* float_func()
{
  float f = 4.0, temp;
  int i = 0;
  for (i = 0; i < 1024; i++)
  {
    temp = f;
    f = f * f;
    f = f / temp;
  }
  return NULL;
}

void* int_func()
{ 
  int f = 7, temp;
  int i = 0;
  fiber_yield();
  for (i = 0; i < 1024; i++)
  {
    temp = f;
    f = f * f;
    f = f / temp;
  }
  return NULL;
}

void* benchmark_func()
{
  int a[1024], i;
  for (i = 0; i < 1024; i++)
    a[i] = 1;
  return NULL;
}

int main(int argc, char* argv[])
{
  fiber_t* client1, *client2, *client3;
  fiber_manager_init(2);
  if (argv[1] != NULL && !strcmp(argv[1], "all"))
  {
    client1 = fiber_create(10240, &benchmark_func, NULL);
    fiber_join(client1, NULL);
    printf("Benchmark tested !\n");
  }
  else
  {
    client1 = fiber_create(10240, &float_func, NULL);
    client2 = fiber_create(10240, &int_func, NULL);
    client3 = fiber_create(10240, &float_func, NULL);
    fiber_join(client1, NULL);
    fiber_join(client2, NULL);
    fiber_join(client3, NULL);
    printf("Test functions tested !\n");
  }

  return 0;
}
