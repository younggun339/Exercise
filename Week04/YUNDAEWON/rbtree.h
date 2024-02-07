#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree    *new_rbtree(void);
void      delete_rbtree(rbtree *);

node_t    *rbtree_insert(rbtree *, const key_t);
node_t    *rbtree_find(const rbtree *, const key_t);
node_t    *rbtree_min(const rbtree *);
node_t    *rbtree_max(const rbtree *);
int       rbtree_erase(rbtree *, node_t *);

int       rbtree_to_array(const rbtree *, key_t *, const size_t);


/* insert.c*/
void      insert_fixed(rbtree *,node_t *);
void      insert_binary(rbtree *, node_t *);


/* erase.c */
node_t    *tree_minimum(rbtree *, node_t *);
void      erase_fixed(rbtree *,node_t *);
void      replace_node(rbtree *,node_t *,node_t *);
void      *erase_binary(rbtree *, node_t *);
void      delete_node(rbtree *, node_t *);
void      delete_fixup(rbtree*,node_t*);

/* util.c */
node_t      *get_grandparent(rbtree *,node_t *);
node_t      *get_uncle(rbtree *, node_t *);
void        rotate_left(rbtree *, node_t *);
void        rotate_right(rbtree *, node_t *);
void        free_inorder_traversal(rbtree *, node_t *);
node_t      *get_sibling(rbtree*, node_t *);
void        to_arr_inorder_traversal(rbtree *, node_t *, key_t *, size_t *);



#endif  // _RBTREE_H_
