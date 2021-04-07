#pragma once

#include "types.h"

char *hi_heap_init(void);

void *hi_heap_malloc(size_t size);

void hi_heap_free(void *p);
