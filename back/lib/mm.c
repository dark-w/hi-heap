#include "list.h"
#include "mm.h"

#include <stdlib.h>

#include <stdio.h>

#define LIST_NODE_SIZE WORD_ALIGN_UP(sizeof(struct list_head))
#define LIST_NODE_ALIGN(size)                                                  \
    (((size) + LIST_NODE_SIZE - 1) & ~(LIST_NODE_SIZE - 1))
#define MIN_HEAP_LEN 512
#define IS_FREE(size) (((size) & (WORD_SIZE - 1)) == 0)
#define GET_SIZE(region) ((region)->curr_size & ~(WORD_SIZE - 1))

struct mem_region {
    size_t pre_size;
    size_t curr_size;
    struct list_head ln_mem_region;
};

static LIST_HEAD(g_free_region_list);

static inline struct mem_region *get_successor(struct mem_region *region)
{
    size_t size = GET_SIZE(region);
    struct mem_region *r =
        (struct mem_region *)((__u8 *)region + DWORD_SIZE + size);
    return r;
}

static inline struct mem_region *get_predeccessor(struct mem_region *region)
{
    return (struct mem_region *)((__u8 *)region -
                                 (region->pre_size & ~(WORD_SIZE - 1)) -
                                 DWORD_SIZE);
}

static void inline region_set_size(struct mem_region *region, size_t size)
{
    struct mem_region *succ_region;

    region->curr_size = size;

    succ_region = get_successor(region);
    succ_region->pre_size = size;
}

static int __heap_init(unsigned long start, unsigned long end)
{
    struct mem_region *first, *tail;

    start = WORD_ALIGN_UP(start);
    end = WORD_ALIGN_DOWN(end);

    if (start + MIN_HEAP_LEN >= end)
        return -1;

    first = (struct mem_region *)start;
    tail = (struct mem_region *)(end - DWORD_SIZE); // sizeof(*tail)

    first->pre_size = 1;
    first->curr_size = (unsigned long)tail - (unsigned long)first - DWORD_SIZE;

    tail->pre_size = first->curr_size;
    tail->curr_size = 1;

    list_add_tail(&first->ln_mem_region, &g_free_region_list);

    return 0;
}

static char *heap;

int heap_init(size_t size)
{
    heap = (char *)malloc(size);

    unsigned long heap_start = (unsigned long)heap;
    unsigned long heap_end = (unsigned long)heap + CONFIG_HEAP_SIZE;

    return __heap_init(heap_start, heap_end);
}

void *my_malloc(size_t size)
{
    void *p = NULL;
    struct list_head *iter;
    size_t alloc_size, reset_size;
    struct mem_region *curr_region, *succ_region;

    alloc_size = LIST_NODE_ALIGN(size);
    list_for_each (iter, &g_free_region_list) {
        curr_region = container_of(iter, struct mem_region, ln_mem_region);
        if (curr_region->curr_size >= alloc_size)
            goto do_alloc;
    }

    return NULL;

do_alloc:
    list_del(iter);

    reset_size = curr_region->curr_size - alloc_size;

    if (reset_size < sizeof(struct mem_region))
        region_set_size(curr_region, curr_region->curr_size | 1);
    else {
        region_set_size(curr_region, alloc_size | 1);

        succ_region = get_successor(curr_region);
        region_set_size(succ_region, reset_size - DWORD_SIZE);
        list_add_tail(&succ_region->ln_mem_region, &g_free_region_list);
    }

    p = &curr_region->ln_mem_region;

    return p;
}

void my_free(void *p)
{
    struct mem_region *curr_region, *succ_region;

    curr_region = (struct mem_region *)((unsigned long)p - DWORD_SIZE);
    succ_region = get_successor(curr_region);

    if (IS_FREE(succ_region->curr_size)) {
        region_set_size(curr_region, GET_SIZE(curr_region) +
                                         succ_region->curr_size + DWORD_SIZE);
        list_del(&succ_region->ln_mem_region);
    } else
        region_set_size(curr_region, GET_SIZE(curr_region));

    if (IS_FREE(curr_region->pre_size)) {
        struct mem_region *prev_region;

        prev_region = get_predeccessor(curr_region);
        region_set_size(prev_region, prev_region->curr_size +
                                         curr_region->curr_size + DWORD_SIZE);
    } else
        list_add_tail(&curr_region->ln_mem_region, &g_free_region_list);
}

struct list_head *get_heap_head_list(void)
{
    return &g_free_region_list;
}

#if 1
void mm_test(void)
{
	heap_init(1024);
	int *p = (int *)my_malloc(sizeof(*p) * 3); // 248 * 4

	for (int i = 0; i < 3; i++) {
		p[i] = i * 2;
	}

	for (int i = 0; i < 3; i++) {
		printf("p[%d]: %d\n", i, p[i]);
	}

	my_free(p);

	free(heap);
}
#endif
