#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "unity.h"
#include "memory.h"

void test_copy_block()
{
    // Create source block
    t_block src_block = (t_block)malloc(sizeof(struct s_block) + 100);
    src_block->size = 100;
    src_block->free = 0;
    src_block->next = NULL;
    src_block->ptr = src_block->data;

    // Fill source block with data
    int *src_data = (int *)src_block->ptr;
    for (int i = 0; i < 25; i++)
    {
        src_data[i] = i;
    }

    // Create destination block
    t_block dst_block = (t_block)malloc(sizeof(struct s_block) + 100);
    dst_block->size = 100;
    dst_block->free = 0;
    dst_block->next = NULL;
    dst_block->ptr = dst_block->data;

    // Copy data from source block to destination block
    copy_block(src_block, dst_block);

    // Check if data was copied correctly
    int *dst_data = (int *)dst_block->ptr;
    for (int i = 0; i < 25; i++)
    {
        assert(dst_data[i] == i);
    }

    printf("test_copy_block passed\n");

    // Free the allocated memory
    free(src_block);
    free(dst_block);
}

void test_split_block()
{
    // Create a block of memory
    t_block block = (t_block)malloc(sizeof(struct s_block) + 100);
    block->size = 100;
    block->free = 0;
    block->next = NULL;

    // Split the block
    split_block(block, 40);

    // Check the properties of the original block
    assert(block->size == 40);
    assert(block->free == 0);
    assert(block->next != NULL);

    // Check the properties of the new block
    t_block new_block = block->next;
    assert(new_block->size == 100 - 40 - 40);
    assert(new_block->free == 1);
    assert(new_block->next == NULL);

    printf("test_split_block passed\n");

    // Free the allocated memory
    free(block);
}

int main()
{
    test_split_block();
    test_copy_block();
    return 0;
}

