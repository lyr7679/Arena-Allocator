#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// This is the control benchmark to test the c malloc
#define MAX 1000
void *pointers[MAX] = {};
int main( int argc, char * argv[] )
{
  clock_t t, t1, t2, t3;
  t = clock();
  mavalloc_init(300000000, NEXT_FIT);
  t1 = clock();
  for (int i = 0; i < MAX; i++) {
    pointers[i] = mavalloc_alloc(10000);
  }
  t1 = clock() - t1;
  t2 = clock();
  for(int i = 0; i < MAX; i+=2) {
    mavalloc_free(pointers[i]);
  }
  t2 = clock() - t2;
  t3 = clock();
  for(int i = 0; i < MAX; i++) {
    pointers[i] = mavalloc_alloc(5000);
  }
  t3 = clock() - t3;
  t = clock() - t;

  printf("Time for first allocation to complete: %0.2f clocks\n", (double)t1);
  printf("Time for free to complete: %0.2f clocks\n", (double)t2);
  printf("Time second allocation to complete: %0.2f clocks\n", (double)t3);
  printf("Time for next fit to complete: %0.2f clocks\n", (double)t);

  return 0;
}
