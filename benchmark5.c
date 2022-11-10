#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// WORST FIT

#define MAX 10
int allocations[] = {0, 12, 65, 120, 1000, 2000, 5000, 10000, 3000, 2000};

int main( int argc, char * argv[] )
{
  // mavalloc_init(24000, WORST_FIT);
  // clock_t t;
  // for (int i = 0; i < MAX; i++) {
  //   t = clock();
  //   mavalloc_alloc(allocations[i]);
  //   t = clock() - t;
  //   printf("Time for WORST FIT mavalloc of size %d to complete: %0.2f ms\n", allocations[i], (double)t);
  // }

  // printArr();

  mavalloc_init( 71608, WORST_FIT );
  char * ptr1    = ( char * ) mavalloc_alloc ( 65535 );
  char * buffer1 = ( char * ) mavalloc_alloc( 4 );
  char * ptr4    = ( char * ) mavalloc_alloc ( 64 );
  char * buffer2 = ( char * ) mavalloc_alloc( 4 );
  char * ptr2    = ( char * ) mavalloc_alloc ( 6000 );

  printArr();

  // If you failed here your allocation on line 206 failed
  //TINYTEST_ASSERT( ptr1 );

  // If you failed here your allocation on line 210 failed
  //TINYTEST_ASSERT( ptr2 );

  // If you failed here your allocation on line 208 failed
  //TINYTEST_ASSERT( ptr4 );

  mavalloc_free( ptr1 );
  mavalloc_free( ptr2 );

  buffer1 = buffer1;
  buffer2 = buffer2;
  ptr4 = ptr4;

  char * ptr3 = ( char * ) mavalloc_alloc ( 1000 );

  // If you failed here then your worst fit picked the wrong node on line 228
  //TINYTEST_EQUAL( ptr1, ptr3 );
  mavalloc_destroy( );
  
  return 0;
}
