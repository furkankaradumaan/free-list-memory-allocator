#include <stdbool.h>
#include <stdio.h>

#include "../include/allocator.h"

#define ALIGNMENT 8
#define MEMORY_SIZE 4096
#define HEADER_SIZE (sizeof(Block))


typedef unsigned char byte;
/*
 * Header: Contains information about a memory block.
 * */
typedef struct Block {
        size_t size; // block size in bytes
        struct Block *next;
        struct Block *prev;
} Block;

/*
 * This is the memory that will be managed.
 */
STATIC byte memory[MEMORY_SIZE];
STATIC Block *free_list_head = NULL;

/*
 * get_payload_size: Returns the amount of memory that is available
 *                   for given block.
 * */
STATIC size_t get_payload_size(Block *block) {
        return block->size - HEADER_SIZE;
}

/*
 * should_split_block_to_allocate: Determines if it we should split the given block
 *                                 to allocate given size of bytes.
 * */
STATIC bool should_split_block_to_allocate(Block *block, size_t size) {
        return get_payload_size(block) >= ALIGNMENT + size;
}

/*
 * calculate_memory_to_allocate: Calculates the amount of bytes we need in total
 *                               to allocate given size of bytes.
 * */
STATIC size_t calculate_memory_to_allocate(size_t bytes) {
        return (bytes + HEADER_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

/*
 * is_block_suitable: Determines if we can allocate given amount of bytes
 *                    from given block.
 * */
STATIC bool is_block_suitable(Block *block, size_t bytes) {
        return block->size >= bytes;
}

/*
 * find_suitable_block: Finds a block from 'free list' such that
 *                      we can allocate given amount of bytes.
 * */
STATIC Block *find_suitable_block(size_t bytes) {
        Block *curr = free_list_head;

        while (curr != NULL && !is_block_suitable(curr, bytes)) {
                curr = curr->next;
        }

        return curr;
}

/*
 * shift_block: Shift the block to the right given amount of size.
 * */
STATIC void shift_block(Block *block, Block **right_half, size_t left_block_size) {
        *right_half = (Block *)((byte *)block + left_block_size);

        (*right_half)->size = block->size - left_block_size;
        (*right_half)->next = block->next;
        (*right_half)->prev = block->prev;
        if ((*right_half)->next) {
                (*right_half)->next->prev = *right_half;
        }
        if ((*right_half)->prev) {
                (*right_half)->prev->next = *right_half;
        } else {
                free_list_head = *right_half;
        }
}

/*
 * allocate_from_block: Allocates given amount of bytes from given block.
 *                     Splits the block if necessary.
 * */
STATIC void *allocate_from_block(Block *block, size_t size) {
        if (should_split_block_to_allocate(block, size)) {
                Block *right_block;
                shift_block(block, &right_block, size);
                
                block->size = size;
                block->next = NULL;
                block->prev = NULL;

                return (char *)block + HEADER_SIZE;
        } else {
                if (block->prev) {
                        block->prev->next = block->next;
                } else {
                        free_list_head = block->next;
                        free_list_head->prev = NULL;
                }
                if (block->next) {
                        block->next->prev = block->prev;
                }

                block->prev = NULL;
                block->next = NULL;

                return (char *)block + HEADER_SIZE;
        }
}

/**
 *
 * fits_between: Checks whether block curr fits between prev and
 *               next blocks without overlapping.
 */
STATIC bool fits_between(Block *prev, Block *curr, Block *next) {
        if (prev && (byte *)prev + prev->size > (byte *)curr) return false;
        if (next && (byte *)curr + curr->size > (byte *)next) return false;
        return true;
}

/*
 * are_contiguous_blocks: Checks whether block next comes right
 *                        after prev block in memory.
 *                        If either prev or next is NULL, returns false.
 * */
STATIC bool are_contiguous_blocks(Block *prev, Block *next) {
        if (!prev || !next) return false;
        
        return (byte *)prev + prev->size == (byte *)next;
}

STATIC void merge_with_prev(Block *block) {
        if (!block->prev || !block) return;

        if (are_contiguous_blocks(block->prev, block)) {
                // remove 'block'
                block->prev->next = block->next;
                block->prev->size += block->size;
                if (block->next) block->next->prev = block->prev;
        }
}

STATIC void merge_with_next(Block *block) {
        if (!block->next || !block) return;

        if (are_contiguous_blocks(block, block->next)) {
                block->size += block->next->size;
                block->next = block->next->next;
                if (block->next) block->next->prev = block;
        }
}

void memory_init(void) {
        free_list_head = (Block *)memory;
        free_list_head->size = MEMORY_SIZE;
        free_list_head->next = NULL;
        free_list_head->prev = NULL;
}


void *allocate(size_t bytes) {
        if (bytes == 0) return NULL;

        size_t memory_needed = calculate_memory_to_allocate(bytes); 
        Block *block = find_suitable_block(memory_needed);
        if (block == NULL) return NULL;

        void *mem = allocate_from_block(block, memory_needed);
        
        
        return mem;
}

void release(void *ptr) {
        // check if given pointer is NULL or
        // not in 'memory'
        if (ptr == NULL) return;
        if ((byte *)ptr < memory || (byte *)ptr >= memory + MEMORY_SIZE) {
                fprintf(stderr, "Invalid pointer\n");
                return;
        }
        
        Block *block = (Block *)ptr - 1;
        if (free_list_head == NULL) {
                free_list_head = block;
                free_list_head->next = NULL;
                free_list_head->prev = NULL;

                return;
        }

        Block *next = free_list_head;
        Block *prev = NULL;
        while (next != NULL) {
                if (fits_between(prev, block, next)) {
                        block->prev = prev;
                        block->next = next;
                        if (prev) prev->next = block;
                        else free_list_head = block;

                        if (next) next->prev = block;

                        merge_with_next(block);
                        merge_with_prev(block);

                        return;
                }
                prev = next;
                next = next->next;
        }
        if (fits_between(prev, block, next)) {
                block->next = next;
                block->prev = prev;
                if (prev) prev->next = block;

                merge_with_prev(block);

                return;
        }

        fprintf(stderr, "This memory (or a part of the memory) is already in free list.\n");
}

void draw_memory(void) {
        Block *curr_block = free_list_head;
        byte *curr_addr = memory;

        while (curr_addr < memory + MEMORY_SIZE) {
                if (curr_block == NULL) {
                        size_t allocated_block_size = memory + MEMORY_SIZE - curr_addr;
                        printf("[%zu bytes (Allocated)]", allocated_block_size);
                        curr_addr = memory + MEMORY_SIZE;
                } else if (curr_addr < (byte *)curr_block) {
                        size_t allocated_block_size = (byte *)curr_block - curr_addr;
                        printf("[%zu bytes (Allocated)]", allocated_block_size);
                        curr_addr = (byte *)curr_block;
                } else {
                        size_t free_block_size = curr_block->size;
                        printf("[%zu bytes (Free)]", free_block_size);
                        curr_addr = (byte *)curr_block + free_block_size;
                        curr_block = curr_block->next;
                }
        }
        printf("\n");
}
