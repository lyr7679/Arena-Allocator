#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// This is the control benchmark to test the c malloc
#define MAX 4000
int allocations1[MAX];
int allocations2[MAX];
void *pointers[MAX * 2] = {};
int main( int argc, char * argv[] )
{
  srand(time(NULL));
  for(int i = 0; i < MAX; i++) {
    allocations1[i] = rand() % 5000;
    allocations2[i] = rand() % 10000;
  }
  clock_t t;
  t = clock();
  for (int i = 0; i < MAX; i++) {
    pointers[i] = malloc(allocations1[i]);
  }
  for(int i = 0; i < MAX; i+=2) {
    free(pointers[i]);
  }
  for(int i = MAX; i < MAX * 2; i++) {
    pointers[i] = malloc(allocations2[i]);
  }

  t = clock() - t;

  printf("Time for malloc to complete: %0.2f ms\n", (double)t);

  return 0;
}
