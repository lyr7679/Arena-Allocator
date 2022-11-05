#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// BEST FIT
#define MAX 10
int allocations[] = {0, 12, 65, 120, 1000, 2000, 5000, 10000, 3000, 2000};

int main( int argc, char * argv[] )
{
  clock_t t;
  mavalloc_init(24000, BEST_FIT);
  for (int i = 0; i < MAX; i++) {
    t = clock();
    mavalloc_alloc(allocations[i]);
    t = clock() - t;
    printf("Time for BEST FIT mavalloc of size %d to complete: %0.2f ms\n", allocations[i], (double)t);
  }
  printArr();
  
  return 0;
}
