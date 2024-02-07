#include "rbtree.h"

#include <stdlib.h>

node_t		*rbtree_insert(rbtree *t, const key_t key) {
	node_t *n;

	if (!(n= (node_t *)calloc(1, sizeof(node_t))))
		return NULL;

	initialize_insert_node(t, n, key);
	insert_node_binarytree(t, n);
	rbtree_insert_case_check(t, n);
	return n;
}

void		initialize_insert_node(rbtree *t, node_t *n, const key_t key) {
	n->color = RBTREE_RED;
	n->left = t->nil;
	n->right = t->nil;
	n->parent = t->nil;
	n->key = key;
}

void		insert_node_binarytree(rbtree *t, node_t *n) {
	node_t *next = t->root;
	node_t *parent = t->nil;

	while (next != t->nil) {
		parent = next;
		if (n->key < next->key)
			next = next->left;
		else
			next = next->right;
	}
	n->parent = parent;
	if (parent == t->nil)
		t->root = n;
	else if (n->key < parent->key)
		parent->left = n;
	else
		parent->right = n;
}

void		rbtree_insert_case_check(rbtree *t, node_t *n) {
	node_t *U = get_uncle_ptr(t, n);
	node_t *G = get_grandparent_ptr(t, n);

	/*     Case 1      */
	if (t->root == t->nil) {
		t->root = n;
		n->color = RBTREE_BLACK;
	} else {
		/*     Case 2      */
		if (n != t->root && n->parent->color == RBTREE_BLACK) {
			return ;
		} else {
			/*     Case 3      */
			if ((U != NULL) && (U->color == RBTREE_RED)) {
				n->parent->color = RBTREE_BLACK;
				U->color = RBTREE_BLACK;
				G->color = RBTREE_RED;
				rbtree_insert_case_check(t, G);
			} else {
			/*     Case 4      */
				if ((n == n->parent->right) && (n->parent == G->left)) {
					rotate_left(t ,n->parent);
					n = n->left;
				} 
				else if ((n == n->parent->left) && (n->parent == G->right)) {
					rotate_right(t, n->parent);
					n = n->right;
				}
				/*     Case 5      */
				G = get_grandparent_ptr(t, n);
				if (G != t->nil) {
					n->parent->color = RBTREE_BLACK;
					G->color = RBTREE_RED;
					if (n == n->parent->left)
						rotate_right(t, G);
					else
						rotate_left(t, G);
				}
			}
		}
	}
	t->root->color = RBTREE_BLACK;
}
