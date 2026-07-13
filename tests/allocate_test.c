/* This file tests the 'allocate' function and related
 * functions that used to allocate memory. */
#include <assert.h>
#include <stdio.h>

#include "../src/allocator.c"

#define RUN_TEST(test) \
                do { \
                        test() \
                } while (0);

void test_get_payload_size() {
        // Create mock blocks
        const int test_cases = 5;
        size_t expected_values[] = {64, 128, 8, 4096, 256};
        
        for (size_t i = 0; i < test_cases; i++) {
                size_t expected = expected_values[i];
                Block block = {.size = HEADER_SIZE + expected};
                size_t result = get_payload_size(&block);

                if (expected == result) printf("[PASSED]");
                else printf("[FAILED]");
                printf(" test_get_payload_size - expected = %zu - actual = %zu\n",
                                expected, result);
        }
}

void test_should_split_block_to_allocate() {
        const int test_cases = 5;
        Block blocks[] = {{.size = HEADER_SIZE + 128},
                          {.size = HEADER_SIZE + 8},
                          {.size = HEADER_SIZE + 64},
                          {.size = HEADER_SIZE + 256},
                          {.size = HEADER_SIZE + 1024}};
        size_t bytes[] = {HEADER_SIZE + 16,
                          HEADER_SIZE + 4,
                          HEADER_SIZE + 60,
                          HEADER_SIZE + 224,
                          HEADER_SIZE + 1000
                          };
        bool expected_values[] = {true, false, false, true, false};

        for (int i = 0; i < test_cases; i++) {
                bool result = should_split_block_to_allocate(&blocks[i], bytes[i]);
                
                if (expected_values[i] == result) {
                        printf("[PASSED]");
                } else {
                        printf("[FAILED]");
                }
                printf(" test_should_split_block_to_allocate - expected = %d - actual = %d\n",
                                expected_values[i], result);
        }
}

void test_calculate_memory_to_allocate() {
        const int test_cases = 5;
        size_t bytes[] = {18, 4, 10, 16, 8};
        size_t expected_values[] = {HEADER_SIZE + 24,
                                    HEADER_SIZE + 8,
                                    HEADER_SIZE + 16,
                                    HEADER_SIZE + 16, 
                                    HEADER_SIZE + 8};

        for (int i = 0; i < test_cases; i++) {
                size_t size = bytes[i];
                size_t expected = expected_values[i];
                size_t res = calculate_memory_to_allocate(size);
                if(res == expected) {
                        printf("[PASSED]");
                } else {
                        printf("[FAILED]");
                }
                printf(" calculate_memory_to_allocate - expected = %zu - actual = %zu\n",
                        expected, res);
        }
}

void test_is_block_suitable() {
        const int test_cases = 5;
        Block blocks[] = {{.size = HEADER_SIZE + 100},
                          {.size = HEADER_SIZE + 8},
                          {.size = HEADER_SIZE + 128},
                          {.size = HEADER_SIZE + 16},
                          {.size = HEADER_SIZE + 64}};
        size_t bytes[] = {HEADER_SIZE + 99,
                          HEADER_SIZE + 9,
                          HEADER_SIZE + 120,
                          HEADER_SIZE + 10,
                          HEADER_SIZE + 64};
        bool expected_values[] = {true, false, true, true, true};

        for (int i = 0; i < test_cases; i++) {
                bool res = is_block_suitable(&blocks[i], bytes[i]);
                if (res == expected_values[i]) printf("[PASSED]");
                else printf("[FAILED]");
                printf(" is_block_suitable - expected = %d - actual = %d\n",
                                expected_values[i], res);
        }
}

void test_find_suitable_block() {
        Block b1 = {.size = HEADER_SIZE + 8};
        Block b2 = {.size = HEADER_SIZE + 128};
        Block b3 = {.size = HEADER_SIZE + 16};
        Block b4 = {.size = HEADER_SIZE + 16};
        Block b5 = {.size = HEADER_SIZE + 64};
        Block b6 = {.size = HEADER_SIZE + 1024};
        Block b7 = {.size = HEADER_SIZE + 1600};

        b1.prev = NULL; b1.next = &b2; 
        b2.prev = &b1; b2.next = &b3;
        b3.prev = &b2; b3.next = &b4;
        b4.prev = &b3; b4.next = &b5;
        b5.prev = &b4; b5.next = &b6;
        b6.prev = &b5; b6.next = &b7;
        b7.prev = &b6; b7.next = NULL;
        
        free_list_head = &b1;

        int test_cases = 10;
        int sizes[] = {HEADER_SIZE + 16,
                       HEADER_SIZE + 40,
                       HEADER_SIZE + 26,
                       HEADER_SIZE + 129,
                       HEADER_SIZE + 1025,
                       HEADER_SIZE + 1024,
                       HEADER_SIZE + 4,
                       HEADER_SIZE + 17,
                       HEADER_SIZE + 65,
                       HEADER_SIZE + 127};
        Block *expected_values[] = {&b2, &b2, &b2, &b6, &b7, &b6, &b1, &b2, &b2, &b2};

        for (int i = 0; i < test_cases; i++) {
                Block *b = find_suitable_block(sizes[i]);

                if (b == expected_values[i]) printf("[PASSED]");
                else printf("[FAILED]");
                printf(" find_suitable_block\n");
        }
}

void test_shift_block() {
        Block *b1 = (Block *)memory;
        b1->size = HEADER_SIZE + 8;

        Block *b2 = (Block *)((byte *)b1 + b1->size);
        b2->size = HEADER_SIZE + 128;

        Block *b3 = (Block *)((byte *)b2 + b2->size);
        b3->size = HEADER_SIZE + 16;

        Block *b4 = (Block *)((byte *)b3 + b3->size);
        b4->size = HEADER_SIZE + 16;

        Block *b5 = (Block *)((byte *)b4 + b4->size);
        b5->size = HEADER_SIZE + 64;

        Block *b6 = (Block *)((byte *)b5 + b5->size);
        b6->size = HEADER_SIZE + 1024;

        Block *b7 = (Block *)((byte *)b6 + b6->size);
        b7->size = HEADER_SIZE + 1600;

        b1->prev = NULL; b1->next = b2; 
        b2->prev = b1; b2->next = b3;
        b3->prev = b2; b3->next = b4;
        b4->prev = b3; b4->next = b5;
        b5->prev = b4; b5->next = b6;
        b6->prev = b5; b6->next = b7;
        b7->prev = b6; b7->next = NULL;

        free_list_head = b1;

        Block *right_block;
        int left_size = HEADER_SIZE + 16;

        shift_block(b5, &right_block, left_size);
        
        if (((char *)right_block - (char *)b5) == left_size 
            && right_block->next == b6
            && right_block->prev == b4
            && b4->next == right_block
            && b6->prev == right_block) printf("[PASSED]");
        else printf("[FAILED]");
        printf(" test shift_block\n");
}

void test_allocate_from_block() {
        Block *b1 = (Block *)memory;
        b1->size = HEADER_SIZE + 8;

        Block *b2 = (Block *)((byte *)b1 + b1->size);
        b2->size = HEADER_SIZE + 128;

        Block *b3 = (Block *)((byte *)b2 + b2->size);
        b3->size = HEADER_SIZE + 16;

        Block *b4 = (Block *)((byte *)b3 + b3->size);
        b4->size = HEADER_SIZE + 16;

        Block *b5 = (Block *)((byte *)b4 + b4->size);
        b5->size = HEADER_SIZE + 64;

        Block *b6 = (Block *)((byte *)b5 + b5->size);
        b6->size = HEADER_SIZE + 1024;

        Block *b7 = (Block *)((byte *)b6 + b6->size);
        b7->size = HEADER_SIZE + 1600;

        b1->prev = NULL; b1->next = b2; 
        b2->prev = b1; b2->next = b3;
        b3->prev = b2; b3->next = b4;
        b4->prev = b3; b4->next = b5;
        b5->prev = b4; b5->next = b6;
        b6->prev = b5; b6->next = b7;
        b7->prev = b6; b7->next = NULL;

        free_list_head = b1;
        size_t size = calculate_memory_to_allocate(120);

        void *mem = allocate_from_block(b6, size);

        if (((Block *)mem - 1)->size == size) {
                printf("[PASSED]");
        } else {
                printf("[FAILED]");
        }
        printf(" test_allocate_from_block\n");
}

int main() {
        test_get_payload_size();      
        test_calculate_memory_to_allocate();
        test_should_split_block_to_allocate();
        test_is_block_suitable();
        test_find_suitable_block();
        test_shift_block();
        test_allocate_from_block();
        return 0;
}


