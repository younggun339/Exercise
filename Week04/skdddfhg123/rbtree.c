#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p;

  if (!(p = (rbtree *)calloc(1, sizeof(rbtree))))
    return NULL;
  if (!(p->nil = (node_t *)calloc(1, sizeof(node_t)))) {
    free(p);
    return NULL;
  }
  p->nil->color = RBTREE_BLACK;
  p->nil->left = NULL;
  p->nil->right = NULL;
  p->nil->parent = NULL;
  p->nil->key = 0;

  p->root = p->nil;

  return p;
}

static void	free_traversal(rbtree *t, node_t *root) {
	if (root != t->nil) {
		free_traversal(t, root->left);
		free(root);
		free_traversal(t, root->right);
		root = NULL;
	}
}

void delete_rbtree(rbtree *t) {
	// TODO: reclaim the tree nodes's memory
  if (t->root != t->nil)
	  free_traversal(t, t->root);
	if (t->nil) {
	  free(t->nil);
	  t->nil = NULL;
	}
	free(t);
	t = NULL;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *ret = t->root;
  while (ret != t->nil) {
    if (ret->key == key)
      return ret;
    if (ret->key > key)
      ret = ret->left;
    else if (ret->key < key)
      ret = ret->right;
  }
  if (ret->key != key)
    return NULL;
  return ret;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *ret = t->root;
  while (ret->left != t->nil)
    ret = ret->left;
  return ret;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *ret = t->root;
  while (ret->right != t->nil)
    ret = ret->right;
  return ret;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int idx = 0;
  rbtree_traversal(t, t->root, arr, &idx);
  return 0;
}
