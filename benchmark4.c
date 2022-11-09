#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// BEST FIT
#define MAX 10
int allocations[] = {0, 12, 65, 120, 1000, 2000, 5000, 10000, 3000, 2000};
void * pointer_arr[10] = {};

int main( int argc, char * argv[] )
{
  clock_t t;
  mavalloc_init(24000, BEST_FIT);
  for (int i = 0; i < MAX; i++) {
    t = clock();
    pointer_arr[i] = mavalloc_alloc(allocations[i]);
    t = clock() - t;
    printf("Time for BEST FIT mavalloc of size %d to complete: %0.2f ms\n", allocations[i], (double)t);
  }

  mavalloc_free(pointer_arr[5]);
  mavalloc_free(pointer_arr[7]);
  //mavalloc_alloc(2000);

  printArr();
  
  return 0;
}
