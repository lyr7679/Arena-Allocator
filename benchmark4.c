#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// This is the control benchmark to test the c malloc
#define MAX 4000
int allocations1[MAX];
int allocations2[MAX];
void *pointers[MAX] = {};
int main( int argc, char * argv[] )
{
  srand(time(NULL));
  for(int i = 0; i < MAX; i++) {
    allocations1[i] = rand() % 15000;
    allocations2[i] = rand() % 5000;
  }
  clock_t t, t1, t2, t3;
  t = clock();
  mavalloc_init(30000000, BEST_FIT);
  t1 = clock();
  for (int i = 0; i < MAX; i++) {
    pointers[i] = mavalloc_alloc(allocations1[i]);
  }
  t1 = clock() - t1;
  t2 = clock();
  for(int i = 0; i < MAX; i+=2) {
    mavalloc_free(pointers[i]);
  }
  t2 = clock() - t2;
  t3 = clock();
  for(int i = 0; i < MAX; i++) {
    pointers[i] = mavalloc_alloc(allocations2[i]);
  }
  t3 = clock() - t3;
  t = clock() - t;

  printf("Time for first allocation to complete: %0.2f clocks\n", (double)t1);
  printf("Time for free to complete: %0.2f clocks\n", (double)t2);
  printf("Time second allocation to complete: %0.2f clocks\n", (double)t3);
  printf("Time for best fit to complete: %0.2f clocks\n", (double)t);

  return 0;
}
