#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  t->nil = (node_t *)calloc(1, sizeof(node_t));
  t->nil->color = RBTREE_BLACK;
  t->root = t->nil;
  return t;
}

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
void rotate_left(rbtree *t, node_t *curr) {
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
void rotate_right(rbtree *t, node_t *curr) {
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
  if(curr == NULL) {
    return;
  }

  node_t *uncle;

  while(curr->parent != NULL && curr->parent->color == RBTREE_RED) {
    if(curr->parent->parent != NULL && curr->parent == curr->parent->parent->right) {
      uncle = curr->parent->parent->left;
      if(uncle != NULL && uncle->color == RBTREE_RED) {
        uncle->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_BLACK;
        curr->parent->parent->color = RBTREE_RED;
        curr = curr->parent->parent;
      } else {
        if(curr == curr->parent->left) {
          curr = curr->parent;
          rotate_right(t, curr);
        }
        curr->parent->color = RBTREE_BLACK;
        if(curr->parent->parent != NULL) {
          curr->parent->parent->color = RBTREE_RED;
          rotate_left(t, curr->parent->parent);
        }
      }
    } else {
      uncle = curr->parent->parent->right;

      if(uncle != NULL && uncle->color == RBTREE_RED){
        uncle->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_BLACK;
        curr->parent->parent->color = RBTREE_RED;
        curr = curr->parent->parent;
      } else {
        if(curr == curr->parent->right) {
          curr = curr->parent;
          rotate_left(t, curr);
        }
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

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;
  node->color = RBTREE_RED;
  node->parent = t->nil;
  node->left = t->nil;
  node->right = t->nil;

  node_t *parent_position = t->nil;
  node_t *insert_position = t->root;

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

  if(node->parent == t->nil) {
    node->color = RBTREE_BLACK;
    return node;
  }
  if(node->parent->parent == t->nil) {
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

void transplant(rbtree *t, node_t *node_del, node_t *node_sub) {
  if(node_del == NULL || node_sub == NULL) {
    return;
  }

  if(node_del->parent == t->nil) {
    t->root = node_sub;
  } else if(node_del == node_del->parent->left) {
    node_del->parent->left = node_sub;
  } 
  else 
  {
    node_del->parent->right = node_sub;
  }

  node_sub->parent = node_del->parent;
}

node_t *subtree_find_min(rbtree *t, node_t *curr) 
{
  if(curr == NULL) 
  {
    return NULL;
  }

  while(curr->left != t->nil) 
  {
    curr = curr->left;
  }
  return curr;
}
void erase_fix(rbtree *t, node_t *curr) 
{
  if(curr == NULL) 
  {
    return;
  }

  node_t *sibling;

  while(curr != t->root && curr->color == RBTREE_BLACK) 
  {
    if(curr->parent && curr == curr->parent->left) 
    {
      sibling = curr->parent->right;
      if(sibling && sibling->color == RBTREE_RED) 
      {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        rotate_left(t, curr->parent);
        sibling = curr->parent->right;
      }
      if(sibling && sibling->left && sibling->right && sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) 
      {
        sibling->color = RBTREE_RED;
        curr = curr->parent;
      } 
      else 
      {
        if(sibling && sibling->right && sibling->right->color == RBTREE_BLACK) 
        {
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          rotate_right(t, sibling);
          sibling = curr->parent->right;
        }
        sibling->color = curr->parent->color;
        curr->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        rotate_left(t, curr->parent);
        curr = t->root;
      }
    } 
    else 
    {
      sibling = curr->parent->left;
      if(sibling->color == RBTREE_RED) 
      {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        rotate_right(t, curr->parent);
        sibling = curr->parent->left;
      }
      if(sibling && sibling->right && sibling->left && sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) 
      {
        sibling->color = RBTREE_RED;
        curr = curr->parent;
      } 
      else 
      {
        if(sibling && sibling->left && sibling->left->color == RBTREE_BLACK) 
        {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          rotate_left(t, sibling);
          sibling = curr->parent->left;
        }
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

int rbtree_erase(rbtree *t, node_t *curr) 
{
  if(curr == NULL) 
  {
    return -1;
  }

  node_t *node_rep;
  node_t *node_ref = curr;
  int o_color = node_ref->color;

  if(curr->left == t->nil) 
  {
    node_rep = curr->right;
    if(node_rep) 
    {
      transplant(t, curr, curr->right);
    }
  } 
  else if (curr->right == t->nil) 
  {
    node_rep = curr->left;
    if(node_rep) 
    {
      transplant(t, curr, curr->left);
    }
  } 
  else 
  {
    node_ref = subtree_find_min(t, curr->right);
    if(node_ref) 
    {
      o_color = node_ref->color;
      node_rep = node_ref->right;

      if(node_rep) 
      {
        if(node_ref->parent == curr) 
        {
          node_rep->parent = node_ref;
        } else 
        {
          transplant(t, node_ref, node_ref->right);
          node_ref->right = curr->right;
          if(node_ref->right) 
          {
            node_ref->right->parent = node_ref;
          }
        }
      }
      transplant(t, curr, node_ref);
      node_ref->left = curr->left;
      if(node_ref->left) 
      {
        node_ref->left->parent = node_ref;
      }
      node_ref->color = curr->color;
    } 
  }

  if(o_color == RBTREE_BLACK) 
  {
    erase_fix(t, node_rep);
  }
  free(curr);

  return 0;
}

void inorder_trav(const rbtree *t, key_t *arr, const size_t n, node_t *node, int *count) 
{
  if(node == NULL) 
  {
    return;
  }

  if(node == t->nil) 
  {
    return;
  }

  inorder_trav(t, arr, n, node->left, count);
  if(*count < n)
  {
    arr[*count] = node->key;
    *count = *count + 1 ;
  }
  inorder_trav(t, arr, n, node->right, count);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  int count = 0;
  node_t *start = t->root;
  inorder_trav(t, arr, n, start, &count);
  return 0;
}
