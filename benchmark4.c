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
  // clock_t t;
  // mavalloc_init(24000, BEST_FIT);
  // for (int i = 0; i < MAX; i++) {
  //   t = clock();
  //   pointer_arr[i] = mavalloc_alloc(allocations[i]);
  //   printf("%p\n", pointer_arr[i]);
  //   t = clock() - t;
  //   printf("Time for BEST FIT mavalloc of size %d to complete: %0.2f ms\n", allocations[i], (double)t);
  // }

  // mavalloc_free(pointer_arr[3]);
  // mavalloc_free(pointer_arr[4]);
  // //mavalloc_alloc(2000);

  // printArr();
  // mavalloc_destroy();
  // printf("---------------------------");
  // printArr();

  mavalloc_init( 128000, BEST_FIT );

  char * ptr1    = (char*)mavalloc_alloc( 65535 );
  char * ptr2    = (char*)mavalloc_alloc( 65 );

  // If you failed here your allocation on line 39 failed
  //TINYTEST_ASSERT( ptr1 );

  // If you failed here your allocation on line 40 failed
  //TINYTEST_ASSERT( ptr2 );

  printArr();

  printf("\n--------------------------\n");

  mavalloc_free( ptr1 );
  mavalloc_free( ptr2 );

  printArr();

  int size = mavalloc_size();
  return 0;
}
