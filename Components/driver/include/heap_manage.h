#ifndef HEAP_MANAGE
#define HEAP_MANAGE

#include "hal_types.h"

extern void heap_init(void);
extern void *heap_alloc(uint16 size);
extern void heap_free(void *ptr);
extern void heap_kick(void);


#endif