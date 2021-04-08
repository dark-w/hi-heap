#pragma once

#include "types.h"

struct hi_heap_hypervisor {
    char *view;
    size_t size;
};

struct hi_heap_hypervisor *hi_heap_init(size_t size);

int hi_heap_shutdown(void);

void *hi_heap_malloc(size_t size);

void hi_heap_free(void *p, size_t size);

int hi_heap_refresh(void);
