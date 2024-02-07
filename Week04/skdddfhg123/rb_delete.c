#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

static node_t *tree_minimum(rbtree *t,node_t *n) {
	node_t *ret = n;
	while (ret->left != t->nil)
		ret = ret->left;
	return ret;
}

static node_t *succesor(rbtree *t, node_t *n) {
	if (n->right != t->nil) {
		node_t *tmp = tree_minimum(t, n->right);
		printf("key = %d\n", tmp->key);
		return tmp;
	}
	node_t *y = n->parent;
	while (y != t->nil && n == y->right) {
		n = y;
		y = y->parent;
	}
	return y;
}

static void transplant(rbtree *t, node_t *u, node_t *v) {
	if (u->parent == t->nil)
		t->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

void	rb_delete(rbtree *t, node_t *n) {
	node_t *x;
	node_t *y = n;

	color_t original_color = y->color;
	if (n->left == t->nil) {
		x = n->right;
		transplant(t, n, n->right);
	} else if (n->right == t->nil) {
		x = n->left;
		transplant(t, n, n->left);
	} else {
		y = tree_minimum(t, n->right);
		original_color = y->color;
		x = y->right;
		if (y->parent == n) {
			x->parent = y;
		}
		else {
			transplant(t, y, y->right);
			y->right = n->right;
			y->right->parent = y;
		}
		transplant(t, n, y);
		y->left = n->left;
		y->left->parent = y;
		y->color = n->color;
	}
	if (original_color == RBTREE_BLACK)
		rb_delete_fixup(t, x);
	free(n);
}

void rb_delete_fixup(rbtree *t, node_t *x) {
	while (x != t->root && x->color == RBTREE_BLACK) {
		if (x == x->parent->left) {
			node_t *s = x->parent->right;
			/* Case 1 */
			if (s->color == RBTREE_RED) {
				s->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				rotate_left(t, x->parent);
				s = x->parent->right;
			}
			/* Case 2 */
			if (s->left->color == RBTREE_BLACK && s->right->color == RBTREE_BLACK) {
				s->color = RBTREE_RED;
				x = x->parent;
			/* Case 3 */
			} else  {
				if (s->right->color == RBTREE_BLACK) {
					s->left->color = RBTREE_BLACK;
					s->color = RBTREE_RED;
					rotate_right(t, s);
					s = x->parent->right;
				}
				/* Case 4 */
				s->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				s->right->color = RBTREE_BLACK;
				rotate_left(t, x->parent);
				x = t->root;
			}
		}
		/* right */
		else {
			/* Case 1 */
			node_t *s = x->parent->left;
			if (s->color == RBTREE_RED) {
				s->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				rotate_right(t, x->parent);
				s = x->parent->left;
			}
			/* Case 2 */
			if (s->left->color == RBTREE_BLACK && s->right->color == RBTREE_BLACK) {
				s->color = RBTREE_RED;
				x = x->parent;
			/* Case 3 */
			} else {
				if (s->left->color == RBTREE_BLACK) {
					s->right->color = RBTREE_BLACK;
					s->color = RBTREE_RED;
					rotate_left(t, s);
					s = x->parent->left;
				}
				/* Case 4 */
				s->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				s->left->color = RBTREE_BLACK;
				rotate_right(t, x->parent);
				x = t->root;
			}
		}
	}
	x->color = RBTREE_BLACK;
}

// static node_t *tree_delete(rbtree *t, node_t *n) {
// 	node_t *ret;
// 	if (n->left == t->nil) {
// 		ret = n->right;
// 		transplant(t, n, n->right);
// 		return ret;
// 	}
// 	else if (n->right == t->nil) {
// 		ret = n->left;
// 		transplant(t, n, n->left);
// 		return ret;
// 	}
// 	else {
// 		node_t *y = succesor(t, n);
// 		ret = y->right;
// 		if (y->parent != n) {
// 			transplant(t, y, y->right);
// 			y->right = n->right;
// 			y->right->parent = y;
// 		}
// 		transplant(t, n, y);
// 		y->left = n->left;
// 		y->left->parent = y;
// 		return ret;
// 	}
// }

int rbtree_erase(rbtree *t, node_t *p) {
	// TODO: implement erase
	// BST delete
	rb_delete(t, p);
	// node_t *tmp;
	// delete_node_binarytree(t, p);
	// if (p == t->root){
	// 	t->root = t->nil;
	// } else 
	// 	delete_one_child(t, tmp);
	return 0;
}



void delete_node_binarytree(rbtree *t, node_t *n) {
	node_t *d = n;
	// if (d == t->root)
	// 	return t->root;
	if (d->right != t->nil) {
		d = n->right;
		while (d->left != t->nil)
			d = d->left;
	} else if (d->left != t->nil) {
		d = n->left;
		while (d->left != t->nil)
			d = d->left;
	}
	if (d->parent != t->nil) {
		if (d == d->parent->left)
			d->parent->left = t->nil;
		else
			d->parent->right = t->nil;
	}
	n->key = d->key;
	free(d);
	d = NULL;
}

void delete_one_child(rbtree *t, node_t *n)
{
	node_t *child = (n->right == t->nil) ? n->left: n->right;

	if (n == t->root)
		return ;
	replace_node(n, child);
	if (n->color == RBTREE_BLACK) {
		if (child->color == RBTREE_RED)
			child->color = RBTREE_BLACK;
		else 
			delete_case(t, n);
	}
}

void	delete_case(rbtree *t, node_t *n) {
	if (n->parent == t->nil)
		return ;
	node_t *s = sibling(n);
	printf("%d\n", s->key);
	if (s->color == RBTREE_RED) {
		n->parent->color = RBTREE_RED;
		s->color = RBTREE_BLACK;
		if (n == n->parent->left)
			rotate_left(t, n->parent);
		else
			rotate_right(t, n->parent);
	}
	if ((n->parent->color == RBTREE_BLACK) &&
		(s->color == RBTREE_BLACK) &&
		(s->left->color == RBTREE_BLACK) &&
		(s->right->color == RBTREE_BLACK)) {
		s->color = RBTREE_RED;
		delete_case(t, n->parent);
		} else {
			if ((n->parent->color == RBTREE_RED) &&
				(s->color == RBTREE_BLACK) &&
				(s->left->color == RBTREE_BLACK) &&
				(s->right->color == RBTREE_BLACK)) {
				s->color = RBTREE_RED;
				n->parent->color = RBTREE_BLACK;
			} else {
				if  (s->color == RBTREE_BLACK) {
					if ((n == n->parent->left) &&
						(s->right->color == RBTREE_BLACK) &&
						(s->left->color == RBTREE_RED)) { /* this last test is trivial too due to cases 2-4. */
						s->color = RBTREE_RED;
						s->left->color = RBTREE_BLACK;
						rotate_right(t, s);
					} else if ((n == n->parent->right) &&
						(s->left->color == RBTREE_BLACK) &&
						(s->right->color == RBTREE_RED)) {/* this last test is trivial too due to cases 2-4. */
						s->color = RBTREE_RED;
						s->right->color = RBTREE_BLACK;
						rotate_left(t, s);
				}
			}
			s = sibling;
			n->parent->color = RBTREE_BLACK;
			s->color = n->parent->color;

			if (n == n->parent->left) {
				s->right->color = RBTREE_BLACK;
				rotate_left(t, n->parent);
			} else {
				s->left->color = RBTREE_BLACK;
				rotate_right(t, n->parent);
			}
		}
	}
}
