#include "hi-heap-mm.h"
#include "mm.h"
#include "list.h"

#include <stdlib.h>
#include <stdio.h>

static struct hi_heap_hypervisor hypervisor;

struct hi_heap_hypervisor *hi_heap_init(size_t size)
{
    if (heap_init(size)) {
        hypervisor.view = NULL;
        hypervisor.size = 0;
    } else {
        // bit[n] == 0 indicate the bit does not to be used. 
        // when it equals to 1...
        // so i use 'calloc' instead of 'malloc'.
        hypervisor.view = (char *)calloc(size, sizeof(char));
        hypervisor.size = size;
    }

    return &hypervisor;
}

int hi_heap_shutdown(void)
{
    free(hypervisor.view);
    hypervisor.view = NULL;

    hypervisor.size = 0;

    // TODO: real heap free

    return 0;
}

void *hi_heap_malloc(size_t size)
{
    void *ptr = my_malloc(size);

    char *itr = (char *)ptr;
    for (int i = 0; i < size; i++)
        itr[i] = 0xff;

    if (hi_heap_refresh())
        return NULL;

    return ptr;
}

void hi_heap_free(void *p, size_t size)
{
    char *itr = (char *)p;
    for (int i = 0; i < size; i++)
        itr[i] = 0x00;

    my_free(p);

    hi_heap_refresh();
}

int hi_heap_refresh(void)
{
    const char *start = (char *)(get_heap_head_list()->prev);

    if (!start)
        return -1;

    for (int i = 0; i < hypervisor.size; i++)
        hypervisor.view[i] = start[i];

    return 0;
}

void hi_heap_region_show(void)
{
    printf("\n");
    for (int i = 0; i < hypervisor.size; i++) {
        printf("%x", hypervisor.view[i]);

        if (i && 0 == i % 50)
            printf("\n");
    }
    printf("\n");
}
