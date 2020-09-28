#include "pool.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif

void pool_init(pool_t *p, unsigned long element_size, unsigned long page_size)
{
    if (p == NULL) {
        printf("Invalid pool pointer, return\n");
        return;
    }

    p->element_size = max(element_size, sizeof(free_list));
    p->chunk_per_page = page_size / element_size;
    p->used = p->chunk_per_page - 1;
    p->page = -1;
    p->free = NULL;

    p->page_total = 1;
    p->pages_addr = malloc(sizeof(void *) * p->page_total);

    for (int i = 0; i < p->page_total; ++i)
        p->pages_addr[i] = NULL;
}

void *pool_alloc(pool_t *p)
{
    if (p == NULL) {
        printf("Invalid pool pointer, return\n");
        return NULL;
    }

    /* if exist chunk in pool, just take it */
    if (p->free != NULL) {
        free_list *recycle = p->free;
        p->free = p->free->next;
        return recycle;
    }

    p->used++;

    /* a new page is needed and cut to chunk */
    if (p->used >= p->chunk_per_page) {
        p->used = 0;
        p->page++;

        /* more page have to be allocated */
        if (p->page == p->page_total) {
            p->page_total <<= 1;

            void *tmp = realloc(p->pages_addr, sizeof(void *) * p->page_total);

            if (tmp == NULL) {
                fprintf(stderr, "error: pool_alloc(), memory exhausted.\n");
                p->used = p->chunk_per_page;
                p->page--;
                p->page_total >>= 1;
                return NULL;
            } else {
                p->pages_addr = tmp;
            }

            for (int i = p->page_total >> 1; i < p->page_total; i++)
                p->pages_addr[i] = NULL;
        }

        /* allocate memory to page */
        if (p->pages_addr[p->page] == NULL) {
            p->pages_addr[p->page] =
                malloc(p->chunk_per_page * p->element_size);

            if (p->pages_addr == NULL) {
                fprintf(stderr, "error: pool_alloc(), memory exhausted.\n");
                p->used = p->chunk_per_page;
                p->page--;
                return NULL;
            }
        }
    }
    return p->pages_addr[p->page] + p->used * p->element_size;
}

void pool_free(pool_t *p, void *ptr)
{
    free_list *pFree = p->free;
    p->free = (struct free_list *) ptr;
    p->free->next = pFree;
}

void pool_free_all(pool_t *p)
{
    for (int i = 0; i < p->page_total; i++) {
        if (p->pages_addr[i] == NULL)
            break;
        else {
            free(p->pages_addr[i]);
            p->pages_addr[i] = NULL;
        }
    }
    free(p->pages_addr);
    p->pages_addr = NULL;
}
