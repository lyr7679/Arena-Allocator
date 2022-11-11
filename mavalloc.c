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

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
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
int first_fit(size_t);
int next_fit(size_t);
int best_fit(size_t);
int worst_fit(size_t);
void checkMerge(int);
void insertNode(int, size_t);
int findRootNode();

//global variables
struct Node arena_arr[MAX_ALLOC] = {};
//indexx is the number denoting the current node
int indexx = 0;
//used in a switch/case for the 4 different fits
enum ALGORITHM curr_alg;
//address of first node
void *head;
//index used for tracking w/ next fit
int nf_indexx = 0;
//flag used to denote if the first free node
//has been found with next fit
int initialized = 0;


int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
    //clearing all global variables so when multiple tests are called
    //old data is not kept
    indexx = 0;
    initialized = 0;
    nf_indexx = 0;
    head = NULL;

    if(size < 0)
        return -1;

    curr_alg = algorithm;
    
    size = ALIGN4(size);

    head = malloc(size);

    arena_arr[0].arena = head;
    arena_arr[0].size = size;
    arena_arr[0].type = H;

    //it's the initial mem block so it has nothing in front or behind
    arena_arr[0].next = -1;
    arena_arr[0].previous = -1;

    if(arena_arr[0].arena == NULL)
        return -1;

    return 0;
}

//frees initially allocated address to clear array/linked list
void mavalloc_destroy( )
{
  for(int i = 0; i <= indexx; i++) {
    arena_arr[i].arena = NULL;
    arena_arr[i].size = 0;
    arena_arr[i].type = 0;
    arena_arr[i].next = 0;
    arena_arr[i].previous = 0;
  }
  free(head);
  return;
}

void * mavalloc_alloc( size_t size )
{
    void * new_ptr = NULL;
    int indexOfHole;
    size = ALIGN4(size);

    switch(curr_alg)
        {
        case NEXT_FIT:
        {
            indexOfHole = next_fit(size);
            nf_indexx = indexOfHole;
            if(indexOfHole != -1)
            {
                indexx++;
                insertNode(indexOfHole, size);
                new_ptr = arena_arr[indexx].arena;
            }
            break;
        }
        case FIRST_FIT:
        {
            indexOfHole = first_fit(size);
            if(indexOfHole != -1)
            {
                indexx++;
                insertNode(indexOfHole, size);
                new_ptr = arena_arr[indexx].arena;
            }
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
    if(arena_arr[i].size > 0)
      number_of_nodes++;
  }
  return number_of_nodes;
}

//first fit: insert into first free node found
//always start at beginning of table
int first_fit(size_t size)
{
	int j = findRootNode();
    while(j < MAX_ALLOC && j != -1)
    {
        if (arena_arr[j].type == H && arena_arr[j].size >= size)
        {
            return j;
        }
        else
        {
            j = arena_arr[j].next;
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
    //combining if next node is also a hole
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
    //combining if previous node is also a hole
    if(arena_arr[previous].type == H)
    {
        arena_arr[previous].size += arena_arr[current].size;
        arena_arr[current].size = 0;

        arena_arr[previous].next = arena_arr[current].next;
        temp = arena_arr[current].next;
        arena_arr[temp].previous = previous;
    }
}

//next fit: same as first fit, just tracking where last left off on
//inserting a node so we start search from there
//if nothing found, loop around and try again
//if second loop occurs and still no free node is found, then return -1
int next_fit(size_t size)
{
    //j is used as a current node index
	int j = findRootNode();
    int count = 0;

    //if we've already started next fit, we'll start at the last left off node
    //otherwise we start the search at the root node
    if(initialized == 1)
        j = nf_indexx;
    else
        initialized = 1;

    while(count != 2)
    {
        while(j < MAX_ALLOC && j != -1)
        {
            if (arena_arr[j].type == H && arena_arr[j].size >= size)
            {
                return j;
            }
            else
                j = arena_arr[j].next;
        }
        j = findRootNode();
        count++;
    }
	return -1;
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
  int temp1 = 0;
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].type == H && arena_arr[i].size >= size) {
      if(temp1 <= (arena_arr[i].size - size)) {
        temp1 = arena_arr[i].size - size;
        worstFitIndex = i;
      }
    }
  }
  if(worstFitIndex != -1)
    indexx++;
  return worstFitIndex;
}


void insertNode(int indexOfHole, size_t size)
{
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

// void printArr() {
//   int index = findRootNode();
//   if(index == -1)
//     return;
//   while(arena_arr[index].next != -1) {
//     printf("index: %d\n", index);
//     printf("size: %ld\n", arena_arr[index].size);
//     printf("type: %d\n", arena_arr[index].type);
//     printf("pointer: %p\n", arena_arr[index].arena);
//     printf("next: %d\n", arena_arr[index].next);
//     printf("previous: %d\n\n", arena_arr[index].previous);

//     index = arena_arr[index].next;
//   }
//   printf("index: %d\n", index);
//   printf("size: %ld\n", arena_arr[index].size);
//   printf("type: %d\n", arena_arr[index].type);
//   printf("pointer: %p\n", arena_arr[index].arena);
//   printf("next: %d\n", arena_arr[index].next);
//   printf("previous: %d\n\n", arena_arr[index].previous);
// }

int findRootNode() {
  for(int i = 0; i <= indexx; i++) {
    if(arena_arr[i].previous == -1)
      return i;
  }
  return -1;
}