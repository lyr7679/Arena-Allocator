// The MIT License (MIT)
//
// Copyright (c) 2021, 2022 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES UTA OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// * added best fit and worst fit, think its done but i havent checked edge cases
// * added a print array function, will have to remove declaration in mavalloch.h later
// * added insertnode1, think it works, can use with all fit functions
// * removed the indexx++ in the mavalloc init because it messes with my code, making it add twice and have an empty node at spot 1
// * need a good way to track head of array
// * i think the fit functions should return the index of the hole like i did but up to you (need for insertnode1)
// * also need a function that will free up node that have 0 size, the way insertnode1 works is that even if size is zero for hole
//   it will still stay in arena_arr. this will mess us up when we have to count the nodes, also makes it so that it will have to 
//   travel through the empty node to traverse arena_arr
// * also i only changed benchmark 4 so keep that in mind

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_ALLOC 10000
#define INT_MAX 2147483647

enum TYPE
{
    P,
    H
};

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  int next;
  int previous;
};

//function prototypes
int findFreeNodeInternal(size_t);
void * first_fit(size_t);
void * next_fit(size_t);
int best_fit(size_t);
int worst_fit(size_t);
void checkMerge(int);
void insertNode(int indexOfHole, size_t size);
int findRootNode();
void printArr();

//global variables
struct Node arena_arr[MAX_ALLOC];
int indexx = 0;
enum ALGORITHM curr_alg;
void *head;
int rootNode = 1;
int initialized = 0;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
    size = ALIGN4(size);

    if(size < 0)
        return -1;

    curr_alg = algorithm;

    head = malloc(size);

    arena_arr[0].size = size;
    arena_arr[0].arena = head;
    arena_arr[0].type = H;
    arena_arr[0].next = -1;
    arena_arr[0].previous = -1;
    
    if(arena_arr[0].arena == NULL)
        return -1;

    //insertNode(0, size);

    return 0;
}

void mavalloc_destroy( )
{
  free(head);
  return;
}

void * mavalloc_alloc( size_t size )
{
    void * new_ptr = NULL;
    size = ALIGN4(size);
    int indexOfHole;

    switch(curr_alg)
        {
        case NEXT_FIT:
        {
            new_ptr = next_fit(size);
            break;
        }
        case FIRST_FIT:
        {
            first_fit(size);
            break;
        }
        case BEST_FIT:
        {
          indexOfHole = best_fit(size);
          if(indexOfHole != -1) { 
            insertNode(indexOfHole, size);
            new_ptr = arena_arr[indexx].arena; 
          }    
          break;
        }
        case WORST_FIT:
        {
          indexOfHole = worst_fit(size);
          if(indexOfHole != -1) {
            insertNode(indexOfHole, size); 
            new_ptr = arena_arr[indexx].arena; 
          } 
          break;
        }
    }
  // only return NULL on failure
  return new_ptr;
}

void mavalloc_free( void * ptr )
{
    for(int i = 0; i <= indexx; i++)
    {
        if(arena_arr[i].arena == ptr)
        {
            arena_arr[i].type = H;
            checkMerge(i);
            break;
        }
    }
  return;
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].size > 0 && arena_arr[i].type == P)
      number_of_nodes++;
  }

  return number_of_nodes;
}

//helper func
int findFreeNodeInternal(size_t size)
{
	//int i = 0;
	/**
	 *  Start searching the array beginning at the 0th element
	 *  and once we've found an element not in use we'll drop out
	 *  and return the index that is free.
	*/
	for (int i = 0; i < MAX_ALLOC; i++)
	{
		if (arena_arr[i].type == H && arena_arr[i].size >= size)
		{
			return i;
		}
	}
	return -1;
}

//check previous and next to see if there's a hole
//if there is - merge (always merge into earlier node)
void checkMerge(int current)
{
    int next = arena_arr[current].next;
    int previous = arena_arr[current].previous;
    int temp = 0;
    if(arena_arr[next].type == H)
    {
        //combing size into current node
        arena_arr[current].size += arena_arr[next].size;
        arena_arr[next].size = 0;

        //eliminate pointer connections between node of size zero
        arena_arr[current].next = arena_arr[next].next;
        temp = arena_arr[next].next;
        arena_arr[temp].previous = current;
    }
    else if(arena_arr[previous].type == H)
    {
        arena_arr[previous].size += arena_arr[current].size;
        arena_arr[current].size = 0;

        arena_arr[previous].next = arena_arr[current].next;
        temp = arena_arr[current].next;
        arena_arr[temp].previous = previous;
    }
}

void * first_fit(size_t size)
{
return NULL;
}

void * next_fit(size_t size)
{
    return NULL;
}

int best_fit(size_t size)
{
  int bestFitIndex = -1;
  int temp1 = INT_MAX;
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].type == H && arena_arr[i].size >= size) {
      if(temp1 > (arena_arr[i].size - size)) {
        temp1 = arena_arr[i].size - size;
        bestFitIndex = i;
      }
    }
  }
  if(bestFitIndex != -1)
    indexx++;
  return bestFitIndex;
}

int worst_fit(size_t size)
{
  int worstFitIndex = -1;
  int temp1 = -1;
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].type == H && arena_arr[i].size >= size) {
      if(temp1 < (arena_arr[i].size - size)) {
        temp1 = arena_arr[i].size - size;
        worstFitIndex = i;
      }
    }
  }
  if(worstFitIndex != -1)
    indexx++;
  return worstFitIndex;
}

void insertNode(int indexOfHole, size_t size) {
  size = ALIGN4(size);
  arena_arr[indexx].size = size;
  arena_arr[indexx].type = P;
  arena_arr[indexx].arena = arena_arr[indexOfHole].arena;
  arena_arr[indexx].next = indexOfHole;
  arena_arr[indexx].previous = arena_arr[indexOfHole].previous;

  arena_arr[arena_arr[indexOfHole].previous].next = indexx;

  arena_arr[indexOfHole].size = arena_arr[indexOfHole].size - size;
  arena_arr[indexOfHole].arena = arena_arr[indexOfHole].arena + size;
  arena_arr[indexOfHole].previous = indexx;
}

void printArr() {
  int index = findRootNode();;
  while(arena_arr[index].next != -1) {
    printf("size: %ld\n", arena_arr[index].size);
    printf("type: %d\n", arena_arr[index].type);
    printf("pointer: %p\n", arena_arr[index].arena);
    printf("next: %d\n", arena_arr[index].next);
    printf("previous: %d\n\n", arena_arr[index].previous);

    index = arena_arr[index].next;
  }
  printf("size: %ld\n", arena_arr[index].size);
  printf("type: %d\n", arena_arr[index].type);
  printf("pointer: %p\n", arena_arr[index].arena);
  printf("next: %d\n", arena_arr[index].next);
  printf("previous: %d\n\n", arena_arr[index].previous);
}

int findRootNode() {
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].previous == -1)
      return i;
  }
  return -1;
}