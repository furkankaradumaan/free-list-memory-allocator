#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>

#define TEST_ENV

#ifdef TEST_ENV
#define STATIC
#else
#define STATIC static
#endif

/*
 * memory_init: Initializes the free list.
 *              This function must have been called before
 *              using allocator.
 *              
 *              If you call this after one time, memory will be resetted.
 */
void memory_init(void);

/*
 * allocate: Searchs for an available memory block and allocates
 *           desired amount of memory, if there is a block.
 * */
void *allocate(size_t bytes);

/*
 *  relesase: Puts the givne block back into the
 *            free list.
 *            
 *            If given pointer is not belong to the memory
 *            managed by allocator, prints an error and terminates the program.
 *            
 *            If given memory block is already have been released, prints an error
 *            message and terminates the program.
 * */
void release(void *ptr);

/**
 * draw_memory: Prints a representation of the memory
 *              to see free blocks and allocated blocks in the memory
 *              visually on the screen.
 */
void draw_memory(void);

#endif
