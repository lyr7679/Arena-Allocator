#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// FIRST FIT
#define MAX 10
int allocations[] = {0, 12, 65, 120, 1000, 2000, 5000, 10000, 65535, 128000};

int main( int argc, char * argv[] )
{
  clock_t t;
  for (int i = 0; i < MAX; i++) {
    t = clock();
    mavalloc_init(allocations[i], FIRST_FIT);
    t = clock() - t;
    printf("Time for FIRST FIT mavalloc of size %d to complete: %0.2f ms\n", allocations[i], (double)t);
  }
  
  return 0;
}
