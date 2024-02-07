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

rbtree  *new_rbtree(void);
void    delete_rbtree(rbtree *);

node_t  *rbtree_find(const rbtree *, const key_t);
node_t  *rbtree_min(const rbtree *);
node_t  *rbtree_max(const rbtree *);
int     rbtree_erase(rbtree *, node_t *);

int     rbtree_to_array(const rbtree *, key_t *, const size_t);

/* rb_delete.c */
void	  rb_delete(rbtree *, node_t *);
void    rb_delete_fixup(rbtree *, node_t *);
int     rbtree_erase(rbtree *, node_t *);
void    delete_node_binarytree(rbtree *, node_t *);
void    delete_one_child(rbtree *, node_t *);
void    delete_case(rbtree *, node_t *);

/* rb_insert.c */
node_t  *rbtree_insert(rbtree *, const key_t);
void		insert_node_binarytree(rbtree *, node_t *);
void    rbtree_insert_case_check(rbtree *, node_t *);

/* util.c */
void    initialize_insert_node(rbtree *, node_t *, const key_t);
node_t  *get_grandparent_ptr(rbtree *, node_t *);
node_t  *get_uncle_ptr(rbtree *, node_t *);
void    rotate_left(rbtree *, node_t *);
void    rotate_right(rbtree *, node_t *);
node_t  *sibling(node_t *);
void    replace_node(node_t *, node_t *);
void    rbtree_traversal(rbtree *, node_t *, key_t *, int *);
void    visualize_tree(rbtree *, int );


#endif  // _RBTREE_H_
