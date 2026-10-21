#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

#define POOL_SIZE 1024
#define BLOCK_SIZE 32
#define NUM_SIZE (POOL_SIZE / BLOCK_SIZE)
#define ENTER_CRITICAL()                \
    do                                  \
    { /* Disable Hardware Interrupts */ \
    } while (0)
#define EXIT_CRITICAL()                 \
    do                                  \
    { /* Enable Hardware Interrupts  */ \
    } while (0)

typedef struct
{
    alignas(4) uint8_t memoryPool[POOL_SIZE];
    volatile uint8_t blockStatus[NUM_SIZE];
} MemoryManager;

static MemoryManager mem_manager;

void memory_init(void)
{
    ENTER_CRITICAL();
    for (int i = 0; i < NUM_SIZE; i++)
    {
        mem_manager.blockStatus[i] = 0;
    }
    EXIT_CRITICAL();
}

void *my_malloc(size_t datasize)
{
    if (datasize == 0 || datasize > BLOCK_SIZE)
    {
        return NULL;
    }
    ENTER_CRITICAL();
    for (int i = 0; i < NUM_SIZE; i++)
    {
        if (mem_manager.blockStatus[i] == 0)
        {
            mem_manager.blockStatus[i] = 1;
            EXIT_CRITICAL();
            return (void *)&mem_manager.memoryPool[i * BLOCK_SIZE];
        }
    }

    return NULL;
}

void my_free(void *ptr)
{
    if (ptr == 0)
    {
        return;
    }

    uint8_t *byte_ptr = (uint8_t *)ptr;
    uint8_t *pool_start = &mem_manager.memoryPool[0];
    uint8_t *pool_end = &mem_manager.memoryPool[POOL_SIZE];

    if (byte_ptr < pool_start || byte_ptr >= pool_end)
    {
        return;
    }

    size_t offset = (size_t)(byte_ptr - pool_start);
    if (offset % BLOCK_SIZE != 0)
    {
        return;
    }
    size_t block_index = offset / BLOCK_SIZE;
    ENTER_CRITICAL();
    if (mem_manager.blockStatus[block_index] != 0)
    {
        mem_manager.blockStatus[block_index] = 0;
    }
    EXIT_CRITICAL();
}

int main(void)
{
    memory_init();
    uint8_t *blocks_Full[NUM_SIZE];
    for (int i = 0; i < NUM_SIZE; i++)
    {
        blocks_Full[i] = my_malloc(32);
    }

    uint8_t *extra = my_malloc(16);
    if (extra == NULL)
    {
        printf("The test is passed\n");
    }
    else
    {
        printf("some memory is free");
    }
    if (((uintptr_t)blocks_Full[0]) % 4 == 0)
    {
        printf("Alignment passed\n");
    }
    my_free(NULL);
    uint8_t fake = 10;
    my_free(&fake);
    uint8_t *test_guard_2 = my_malloc(16);
    if (test_guard_2 == NULL)
    {
        printf("The test is passed for guard 2\n");
    }
    my_free(blocks_Full[5]+7);
    uint8_t *test_guard_3=my_malloc(16);
    if (test_guard_3 == NULL)
    {
        printf("The test is passed for guard 3\n");
    }
    my_free(blocks_Full[5]);
    uint8_t *recycled_variable=my_malloc(16);
    if (blocks_Full[5]==recycled_variable)
    {
        printf("The test three is also passed, memory allocation working perfectly");
    }
    
}
