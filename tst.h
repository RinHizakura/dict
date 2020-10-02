#ifndef PREFIX_SEARCH_H
#define PREFIX_SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pool.h"

/* forward declaration of ternary search tree */
typedef struct tst_node tst_node;
/** ternary search tree node. */
typedef struct tst_node {
    char key;               /* char key for node (null for node with string) */
    unsigned refcnt;        /* refcnt tracks occurrence of word (for delete) */
    struct tst_node *lokid; /* ternary low child pointer */
    struct tst_node *eqkid; /* ternary equal child pointer */
    struct tst_node *hikid; /* ternary high child pointer */
} tst_node;


/** tst_ins_del() ins/del copy or reference of 's' from ternary search tree.
 *  insert all nodes required for 's' in tree at eqkid node of leaf. if 'del'
 *  is non-zero deletes 's' from tree, otherwise insert 's' at node->eqkid
 *  with node->key set to the nul-chracter after final node in search path. if
 *  'cpy' is non-zero allocate storage for 's', otherwise save pointer to 's'.
 *  if 's' already exists in tree, increment node->refcnt. (to be used for del).
 *  returns address of 's' in tree on successful insert (or on delete if refcnt
 *  non-zero), NULL on allocation failure on insert, or on successful removal
 *  of 's' from tree.
 */
void *tst_ins_del(tst_node **root, const char *s, const int del, const int cpy);

/** tst_search(), non-recursive find of a string in ternary tree.
 *  returns pointer to 's' on success, NULL otherwise.
 */
void *tst_search(const tst_node *p, const char *s);

/** tst_search_prefix() fills ptr array 'a' with words prefixed with 's'.
 *  once the node containing the first prefix matching 's' is found
 *  tst_suggest() is called to traverse the ternary_tree beginning
 *  at the node filling 'a' with pointers to all words that contain
 *  the prefix upto 'max' words updating 'n' with the number of words
 *  in 'a'. a pointer to the first node is returned on success
 *  NULL otherwise.
 */
void *tst_search_prefix(const tst_node *root,
                        const char *s,
                        char **a,
                        int *n,
                        const int max);

/** tst_traverse_fn(), traverse tree calling 'fn' on each word.
 *  prototype for 'fn' is void fn(const void *, void *). data can
 *  be NULL if unused.
 *
 *  The callback can be implemented as following:
 *
 *  // print each word.
 *  void print_word(const void *node, void *data) {
 *      printf("%s\n", tst_get_string(node));
 *  }
 *
 * Then, invoke as "tst_traverse_fn (root, print_word, NULL);"
 */
void tst_traverse_fn(const tst_node *p,
                     void(fn)(const void *, void *),
                     void *data);

/** free the ternary search tree rooted at p, data storage internal. */
void tst_free_all(tst_node *p);

/** free the ternary search tree rooted at p, data storage external. */
void tst_free(tst_node *p);

/** access functions tst_get_key(), tst_get_refcnt, & tst_get_string().
 *  provide access to struct members through opague pointers availale
 *  to program.
 */
char tst_get_key(const tst_node *node);
unsigned tst_get_refcnt(const tst_node *node);
char *tst_get_string(const tst_node *node);
void tst_init();

pool_t tst_node_pool;
static inline void free_node(tst_node *n)
{
#ifndef MEMPOOL

    free(n);
#else
    pool_free(&tst_node_pool, n);
#endif
}

#endif
