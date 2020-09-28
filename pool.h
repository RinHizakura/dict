#ifndef _POOL_H
#define _POOL_H

#include <stdint.h>

typedef struct free_list {
    struct free_list *next;
} free_list;

typedef struct {
    free_list *free;               // a list to collect freed element
    unsigned long element_size;    // size of each element
    unsigned long chunk_per_page;  // number of element can put in a page
    unsigned int used;             // index of element in page
    unsigned int page;             // current used page of memory pool
    unsigned int page_total;       // total pages avalible in memory pool
    void **pages_addr;
} pool_t;

void pool_init(pool_t *p, unsigned long element_size, unsigned long page_size);
void *pool_alloc(pool_t *p);
void pool_free(pool_t *p, void *ptr);
void pool_free_all(pool_t *p);

#endif
