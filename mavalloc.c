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
int insertNode(int);
int insertNodeInternal(int, int);
void checkMerge(int);
void insertNode1(int indexOfHole, size_t size);
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
    // indexx = 0;
    // rootNode = 1;
    // initialized = 0;

    size = ALIGN4(size);

    if(size < 0)
        return -1;

    curr_alg = algorithm;

    head = malloc(size);

    // arena_arr[0].arena = malloc(ALIGN4(size));
    arena_arr[0].size = size;
    arena_arr[0].arena = head;
    arena_arr[0].type = H;
    //
    //it's the initial mem block so it has nothing in front or behind
    arena_arr[0].next = -1;
    arena_arr[0].previous = -1;
    //
    //initialized++;
    //indexx++;
    //
    if(arena_arr[0].arena == NULL)
        return -1;

    //insertNode(size);

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
          if(indexOfHole != -1)
            insertNode1(indexOfHole, size);     
          break;
        }
        case WORST_FIT:
        {
          indexOfHole = worst_fit(size);
          if(indexOfHole != -1)
            insertNode1(indexOfHole, size);  
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
    //first fit: searches mem for first free hole and inserts
    //starts from beginning of list every time
    int success = -1;
    //int freespot = 0;
    //int freespot = findFreeNodeInternal(i);


    // while(success != 1 && status != -1)
    // {
    //     freespot = findFreeNodeInternal(freespot);
    //     if(freespot == -1)
    //         *status = -1;
    //     else
    //     {
    //         if(arena_arr[freespot].size >= size)
    //         {
    //
    //         }
    //     }
    //
    //
    //
    // }

    success = insertNode(size);
    if(success == 0)
        return arena_arr[indexx].arena;
    else
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

int insertNode(int size)
{
     /** Index into the array where we will put this new node */
	int free_node  =  findFreeNodeInternal(size);

     /** Set current to the rootNode so we start searching from the beginning */
	//int current  =  rootNode;

    /** Set previous to -1 since there nothing before the first node in the list */
	int previous = -1;

    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1;

    /**
     * On the first node being inserted make sure that
     * the root node has been initialized and then
     * set the initialized flag to we don't do this initialization
     * again.
     */
	if (initialized == 0)
	{
		arena_arr[1].previous = -1;
		arena_arr[1].next     =  0;
        arena_arr[1].arena = head;
        arena_arr[0].previous = 1;
		//initialized            =  1;
	}

	if (free_node >= 0 && initialized == 1 && indexx < MAX_ALLOC)
	{
      /**
       * Since this list is sorted, iterate over the linked list and find which node we would
       * fit behind with our value.  Once we have found a spot then the while loop will exit
       * and previous will have the index of the node we will insert behind.
       */
		// while (current >= 0 && (arena_arr[current].type == P) && arena_arr[current].size < size)
		// {
		// 	previous = current;
		// 	current  = arena_arr[current].next;
		// }
        previous = arena_arr[free_node].previous;

        /** If we found a free node and we haven't run off the end of the list */
		if (previous >= -1)
		{
 			ret = insertNodeInternal(previous, free_node);
		}
        /** If we ran off the end of the list then insert on the tail of the list */
		// else if( current == -1 )
		// {
		// 	ret = insertNodeInternal(arena_arr[previous].previous, index);
		// }

    /**
     * Now that our new node is linked in lets set the value and mark this array element
     * as being used.
     */
		arena_arr[indexx].size = size;
		arena_arr[indexx].type = P;
        arena_arr[free_node].size -= size;

        if(previous > -1)
        {
            arena_arr[indexx].arena = (arena_arr[previous].arena + arena_arr[previous].size);
        }
        else
            arena_arr[indexx].arena = head;

        if(initialized == 0)
        {
            //index++;
            initialized = 1;
        }
	}

	return ret;
}


int insertNodeInternal(int previous, int current)
{
	/**
	 *  Initialize our current links to an invalid index
	 *  -1 signifies the end of the list on either end
	 */
 	arena_arr[current].previous = -1;
	arena_arr[current].next = -1;

    indexx++;

    /**
	 * Make sure we have a previous node.  If there
	 * was no previous node then the previous value
	 * would be -1
	 */
	if (previous >= 0)
	{
		int temp;
		/**
		 * Connect the current node with the previous node
		 * if it exists and store off previous.next.
		 * Then set previous.next to current
		 */
		arena_arr[indexx].previous = previous;

		temp = arena_arr[previous].next;
		arena_arr[previous].next = indexx;
		arena_arr[indexx].next = temp;

	}
	// else if ( arena_arr[rootNode].previous == -1 && arena_arr[rootNode].next == -1)
	// {
	// 	/* Do nothing since this is the first node in the linked list. */
	// }
	/**
	 * If we have a previous value -1 we're replacing the root node
	 * so after inserting it make sure to update the rootNode
	 */
	else
	{
		arena_arr[rootNode].previous = indexx;
		arena_arr[indexx].previous = -1;
		arena_arr[indexx].next = rootNode;
		rootNode = indexx;
	}

    if(arena_arr[indexx].arena != NULL)
	   return 0;
    else
        return -1;
}


void insertNode1(int indexOfHole, size_t size) {
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
  int index = 0;
  while(index != indexx + 1) {
    printf("index: %d\n", index);
    printf("size: %ld\n", arena_arr[index].size);
    printf("type: %d\n", arena_arr[index].type);
    printf("pointer: %p\n", arena_arr[index].arena);
    printf("next: %d\n", arena_arr[index].next);
    printf("previous: %d\n\n", arena_arr[index].previous);

    index++;
  }
}