#include <stdio.h>
#include <unistd.h>
 
int main(void) {
  // your code goes here
  printf("\n CPU cpunt is %ld", sysconf(_SC_NPROCESSORS_ONLN));
  return 0;
}
