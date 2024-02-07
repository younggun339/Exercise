#include "rbtree.h"

#include <stdlib.h>
// [MOD] debugging purposes
// #include <stdio.h>
// #include <string.h>

// void delete_node(rbtree *t, node_t *curr);
// void rotate_left(rbtree *t, node_t *curr);
// void rotate_right(rbtree *t, node_t *curr);
// void insert_fix(rbtree *t, node_t *curr);
// void transplant(rbtree *t, node_t *node_del, node_t *node_sub);
// void erase_fix(rbtree *t, node_t *curr);
// void inorder_trav(const rbtree *t, key_t *arr, const size_t n, node_t *node, int *count);

// node_t *rbtree_insert(rbtree *, const key_t);
// node_t *rbtree_find(const rbtree *, const key_t);
// node_t *rbtree_min(const rbtree *);
// node_t *rbtree_max(const rbtree *);
// int rbtree_erase(rbtree *, node_t *);
// int rbtree_to_array(const rbtree *, key_t *, const size_t);

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  t->nil = (node_t *)calloc(1, sizeof(node_t));
  t->nil->color = RBTREE_BLACK;
  t->root = t->nil;
  return t;
}

// [MOD] recursively delete node within the RB Tree
void delete_node(rbtree *t, node_t *curr) {
  if (curr != t->nil) {
    delete_node(t, curr->left);
    delete_node(t, curr->right);
    free(curr);
  }
}

void delete_rbtree(rbtree *t) {
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

// [MOD] rotating a sub-tree left (modular function for insert/delete reuse purposes)
void rotate_left(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return;
  }

  node_t *r = curr->right;
  curr->right = r->left;

  if(r->left != t->nil) {
    r->left->parent = curr;
  }

  r->parent = curr->parent;

  if(curr->parent == t->nil) {
    t->root = r;
  } else if(curr == curr->parent->left) {
    curr->parent->left = r;
  } else {
    curr->parent->right = r;
  }

  r->left = curr;
  curr->parent = r;
}

// [MOD] rotating a sub-tree left (modular function for insert/delete reuse purposes)
void rotate_right(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return;
  }

  node_t *l = curr->left;
  curr->left = l->right;

  if(l->right != t->nil) {
    l->right->parent = curr;
  }

  l->parent = curr->parent;

  if(l->parent == t->nil) {
    t->root = l;
  } else if(curr == curr->parent->right) {
    curr->parent->right = l;
  } else {
    curr->parent->left = l;
  }

  l->right = curr;
  curr->parent = l;
}

void insert_fix(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return;
  }

  node_t *uncle;

  while(curr->parent != NULL && curr->parent->color == RBTREE_RED) {
    // subtree in the right side
    if(curr->parent->parent != NULL && curr->parent == curr->parent->parent->right) {
      uncle = curr->parent->parent->left;
      // case 1r : uncle is red = RECOLOR
      if(uncle != NULL && uncle->color == RBTREE_RED) {
        uncle->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_BLACK;
        curr->parent->parent->color = RBTREE_RED;
        curr = curr->parent->parent;
      } else {
        // case 3r : uncle is black + left subtree = RESTRUCTURE (double rotation)
        if(curr == curr->parent->left) {
          curr = curr->parent;
          rotate_right(t, curr);
        }
        // case 2r : uncle is black + right subtree = RESTRUCTURE (single rotation)
        curr->parent->color = RBTREE_BLACK;
        if(curr->parent->parent != NULL) {
          curr->parent->parent->color = RBTREE_RED;
          rotate_left(t, curr->parent->parent);
        }
      }
    // subtree in the left side
    } else {
      uncle = curr->parent->parent->right;

      // case 1l : uncle is red = RECOLOR
      if(uncle != NULL && uncle->color == RBTREE_RED){
        uncle->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_BLACK;
        curr->parent->parent->color = RBTREE_RED;
        curr = curr->parent->parent;
      } else {
        // case 3l : uncle is black + right subtree = RESTRUCTURE (double rotation)
        if(curr == curr->parent->right) {
          curr = curr->parent;
          rotate_left(t, curr);
        }
        // case 2 : uncle is black + left subtree = RESTRUCTURE (single rotation)
        curr->parent->color = RBTREE_BLACK;
        if(curr->parent->parent != NULL) {
          curr->parent->parent->color = RBTREE_RED;
          rotate_right(t, curr->parent->parent);
        }
      }

      if(curr == t->root) {
        break;
      }
    }

    if(t->root != NULL) {
      t->root->color = RBTREE_BLACK;
    }
  }
}

// [MOD] recoloring and/or restructuring tree to make sure the 5 rules of RBTree are maintained
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;
  node->color = RBTREE_RED;
  node->parent = t->nil;
  node->left = t->nil;
  node->right = t->nil;

  node_t *parent_position = t->nil;
  node_t *insert_position = t->root;

  // finding node insert position
  while(insert_position != t->nil) {
    parent_position = insert_position;
    if(node->key < insert_position->key) {
      insert_position = insert_position->left;
    } else {
      insert_position = insert_position->right;
    }
  }
  node->parent = parent_position;
  if(parent_position == t->nil) {
    t->root = node;
  } else if(node->key < parent_position->key) {
    parent_position->left = node;
  } else {
    parent_position->right = node;
  }

  // checking cases for when insert_fix is not necessary
  if(node->parent == t->nil) {
    node->color = RBTREE_BLACK;
    return node;
  }
  if(node->parent->parent == NULL) {
    return node;
  }

  insert_fix(t, node);
  return node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curr = t->root;

  while(curr != t->nil) {
    if(curr->key > key) {
      curr = curr->left;
    } else if(curr->key < key) {
      curr = curr->right;
    } else {
      return curr;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *min = t->root;

  if(min == t->nil) {
    return NULL;
  }

  while(min->left != t->nil) {
    min = min->left;
  }

  return min;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *max = t->root;

  if(max == t->nil) {
    return NULL;
  }

  while(max->right != t->nil){
    max = max->right;
  }

  return max;
}

// [MOD] node swap for erase operation
void transplant(rbtree *t, node_t *node_del, node_t *node_sub) {
  // seg fault avoidance
  if(node_del == NULL || node_sub == NULL) {
    return;
  }

  if(node_del->parent == t->nil) {
    t->root = node_sub;
  } else if(node_del == node_del->parent->left) {
    node_del->parent->left = node_sub;
  } else {
    node_del->parent->right = node_sub;
  }

  node_sub->parent = node_del->parent;
}

// [MOD] finding min value in the subtree for replacement
node_t *subtree_find_min(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return NULL;
  }

  while(curr->left != t->nil) {
    curr = curr->left;
  }
  return curr;
}

// [MOD] recoloring and/or restructuring tree to make sure the 5 rules of RBTree are maintained
void erase_fix(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return;
  }

  node_t *sibling;

  while(curr != t->root && curr->color == RBTREE_BLACK) {
    // case 1l : curr left & sibling right + black height property violation
    if(curr->parent && curr == curr->parent->left) {
      sibling = curr->parent->right;
      // case 1.1l : sibiling is red
      if(sibling && sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        rotate_left(t, curr->parent);
        sibling = curr->parent->right;
      }
      // case 1.2l: sibling and children are black
      if(sibling && sibling->left && sibling->right && sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        curr = curr->parent;
      } else {
        // case 1.3l: sibling is black, sibling left child is red, sibling right child is black
        if(sibling && sibling->right && sibling->right->color == RBTREE_BLACK) {
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          rotate_right(t, sibling);
          sibling = curr->parent->right;
        }
        // case 1.4l: sibling is black, and sibling right child is red
        sibling->color = curr->parent->color;
        curr->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        rotate_left(t, curr->parent);
        curr = t->root;
      }
    // case 1r : curr right & sibling left + black height property violation
    } else {
      sibling = curr->parent->left;
      // case 1.1r : sibiling is red
      if(sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        rotate_right(t, curr->parent);
        sibling = curr->parent->left;
      }
      // case 1.2r: sibling and children are black
      if(sibling && sibling->right && sibling->left && sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        curr = curr->parent;
      } else {
        // case 1.3r: sibling is black, sibling left child is red, sibling right child is black
        if(sibling && sibling->left && sibling->left->color == RBTREE_BLACK) {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          rotate_left(t, sibling);
          sibling = curr->parent->left;
        }
        // case 1.4r: sibling is black, and sibling right child is red
        sibling->color = curr->parent->color;
        curr->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        rotate_right(t, curr->parent);
        curr = t->root;
      }
    }
  }

  curr->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *curr) {
  // seg fault avoidance
  if(curr == NULL) {
    return -1;
  }

  node_t *node_rep;
  node_t *node_ref = curr;
  int o_color = node_ref->color;

  if(curr->left == t->nil) {
    node_rep = curr->right;
    if(node_rep) {
      transplant(t, curr, curr->right);
    }
  } else if (curr->right == t->nil) {
    node_rep = curr->left;
    if(node_rep) {
      transplant(t, curr, curr->left);
    }
  } else {
    node_ref = subtree_find_min(t, curr->right);
    if(node_ref) {
      o_color = node_ref->color;
      node_rep = node_ref->right;

      if(node_rep) {
        // the successor is the right child of current node
        if(node_ref->parent == curr) {
          node_rep->parent = node_ref;
        } else {
          transplant(t, node_ref, node_ref->right);
          node_ref->right = curr->right;
          if(node_ref->right) {
            node_ref->right->parent = node_ref;
          }
        }
      }
      transplant(t, curr, node_ref);
      node_ref->left = curr->left;
      if(node_ref->left) {
        node_ref->left->parent = node_ref;
      }
      node_ref->color = curr->color;
    }
  }

  if(o_color == RBTREE_BLACK) {
    erase_fix(t, node_rep);
  }
  free(curr);

  return 0;
}

// [MOD] inorder traversal w/ recursion
void inorder_trav(const rbtree *t, key_t *arr, const size_t n, node_t *node, int *count) {
  // seg fault avoidance
  if(node == NULL) {
    return;
  }

  // base case
  if(node == t->nil) {
    return;
  }

  inorder_trav(t, arr, n, node->left, count);
  if(*count < n){
    arr[*count] = node->key;
    *count = *count + 1 ;
  }
  inorder_trav(t, arr, n, node->right, count);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int count = 0;
  node_t *start = t->root;
  inorder_trav(t, arr, n, start, &count);
  return 0;
}

// // [MOD] debugging purposes
// void print_tree(rbtree *t, node_t *node, int depth) {
//   if (node == t->nil) {
//     return;
//   }

//   print_tree(t, node->right, depth + 1);
//   for (int i = 0; i < depth; i++) {
//     printf("     ");
//   }
//   printf("R----%d (%s)\n", node->key, node->color == RBTREE_RED ? "Red" : "Black");
//   print_tree(t, node->left, depth + 1);
// }

// // [MOD] debugging purposes (must be commented for 'make test')
// int main(int argc, char *argv[]) {
//   rbtree *t = new_rbtree();
//   for(int i = 1; i < 11; i++) {
//     rbtree_insert(t, i);
//   }

//   print_tree(t, t->root, 0);

//   node_t *temp = rbtree_find(t, 6);
//   printf("%d\n", temp->color);
//   rbtree_erase(t, temp);

//   printf("after delete\n");
//   print_tree(t, t->root, 0);

//   return 0;
// }
