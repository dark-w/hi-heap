#include "mm.h"
#include "hi-heap-mm.h"

#include <stdio.h>

struct mem_region {
    char *mem;
    size_t size;
};

static void hi_heap_mm_test(size_t size)
{
    // struct mem_region pool[5];
    struct hi_heap_hypervisor *hypervisor = hi_heap_init(size);

    printf("%ld", hypervisor->size);

    hi_heap_region_show();

    char *ptr = (char *)hi_heap_malloc(1);

    hi_heap_region_show();

    hi_heap_free(ptr, 1);

    hi_heap_region_show();

    // for (int i = 0; i < sizeof(pool) / sizeof(pool[0]); i++) {
    //     pool[i].mem = (char *)hi_heap_malloc(i + 1);
    //     pool[i].size = i + 1;

    //     hi_heap_region_show();
    // }

    // for (int i = 0; i < sizeof(pool) / sizeof(pool[0]); i++) {
    //     hi_heap_free(pool[i].mem, pool[i].size);

    //     pool[i].mem = NULL;
    //     pool[i].size = 0;

    //     hi_heap_region_show();
    // }

    hi_heap_shutdown();
}

int main(int argc, char **argv)
{
    hi_heap_mm_test(1024);

	return 0;
}
