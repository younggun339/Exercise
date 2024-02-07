#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1,sizeof(node_t));
  nil->color = RBTREE_BLACK;

  t->nil = t->root = nil;
  return t;
}

void delete_node_memory(rbtree *t, node_t *node){
  if (node->left != t->nil)
  {
    delete_node_memory(t, node->left);
  }
  if (node->right != t->nil)
  {
    delete_node_memory(t, node->right);
  }
  free(node);
}

void left_rotate(rbtree *t, node_t *x) {             
  node_t *y = x->right;
  x->right = y->left;                               
                                                    
                                              
  if (y->left != t->nil) {                            
    y->left->parent = x;                              
  }
  
  y->parent = x->parent;                
  
  if (x->parent == t->nil) {                          
    t->root = y;
  } else if (x == x->parent->left) {                  
    x->parent->left = y;
  } else {                                          
    x->parent->right = y;
  }
  
  y->left = x;                          

  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  
  x->left = y->right;
  
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  
  y->parent = x->parent;
  
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x->parent->right == x) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }
  
  y->right = x;
  x->parent = y;
}

void exchange_color(node_t *a, node_t *b)
{
  int tmp = a->color;
  a->color = b->color;
  b->color = (tmp == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED;
}

void delete_rbtree(rbtree *t) {
  node_t *current = t->root;
  if(current != t->nil) delete_node_memory(t,current);
  free(t->nil);
  free(t);
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {

    if (z->parent == z->parent->parent->left) {       
      node_t *uncle = z->parent->parent->right;
      
      if (uncle->color == RBTREE_RED) {               
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;                        
        } else {
        
        if (z == z->parent->right) {                  
          z = z->parent;                              
          left_rotate(t, z);                 
        }
        
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
      
      } else {                                          
      node_t *uncle = z->parent->parent->left;
      
        if (uncle->color == RBTREE_RED) {
          z->parent->color = RBTREE_BLACK;
          uncle->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            right_rotate(t, z);
          }
          
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          left_rotate(t, z->parent->parent);
        }
    }
  }
  
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {   
  node_t *x = t->root;                                
  node_t *y = t->nil;                                 
  
  while (x != t->nil) {                               
    y = x;                                           
    if (x->key > key) {                               
      x = x->left;
    } else {                                          
      x = x->right;                                   
    }
  }
  
  
  node_t *z = (node_t *)calloc(1, sizeof(node_t));    
  z->key = key;
  z->parent = y;                                      
  
  if (y == t->nil) {                                  
    t->root = z;                                      
  } else if (z->key < y->key) {                      
    y->left = z;
  } else {                                            
    y->right = z;
  }
  
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;                           
  rbtree_insert_fixup(t, z);                          
  
  return t->root;                                  
}  

node_t *rbtree_find(const rbtree *t, const key_t key){
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key < current->key)
    {
      current = current->left;
    }else if(current->key == key){
      return current;
    }else{
      current = current->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t){
  node_t *current = t->root;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t){
  node_t *current = t->root;
  while (current->right != t->nil)
  {
    current = current->right;
  }
  return current;
}

node_t *get_next_node(const rbtree*t, node_t *p){
  node_t *current = p->right;
  if (current == t->nil)
  {
    current = p;
    while (1)
    {
      if (current->parent->right == current) current = current->parent;
      else return current->parent;
    }
  }
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return current;
}

void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left)
{
  // node_t *extra_black = is_left ? parent->left : parent->right;
  // if (extra_black->color == RBTREE_RED)
  // {
  //   extra_black->color = RBTREE_BLACK;
  //   return;
  // }

  // node_t *sibling = is_left ? parent->right : parent->left;
  // node_t *sibling_left = sibling->left;
  // node_t *sibling_right = sibling->right;

  // if (sibling->color == RBTREE_RED)
  // { 
  //   if (is_left)
  //     left_rotate(t, sibling);
  //   else
  //     right_rotate(t, sibling);
  //   exchange_color(sibling, parent);
  //   rbtree_erase_fixup(t, parent, is_left);
  //   return;
  // }

  // node_t *near = is_left ? sibling_left : sibling_right;    
  // node_t *distant = is_left ? sibling_right : sibling_left; 

  // if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  // { 
  //   right_rotate(t, near);
  //   exchange_color(sibling, near);
  //   rbtree_erase_fixup(t, parent, is_left);
  //   return;
  // }

  // if (is_left && distant->color == RBTREE_RED)
  // { 
  //   left_rotate(t, sibling);
  //   exchange_color(sibling, parent);
  //   distant->color = RBTREE_BLACK;
  //   return;
  // }

  // if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  // { 
  //   left_rotate(t, near);
  //   exchange_color(sibling, near);
  //   rbtree_erase_fixup(t, parent, is_left);
  //   return;
  // }

  // if (distant->color == RBTREE_RED)
  // { 
  //   right_rotate(t, sibling);
  //   exchange_color(sibling, parent);
  //   distant->color = RBTREE_BLACK;
  //   return;
  // }

  // sibling->color = RBTREE_RED;

  // if (parent != t->root)
  //   rbtree_erase_fixup(t, parent->parent, parent->parent->left == parent);
}

int rbtree_erase(rbtree *t, node_t *p){
  node_t *remove;
  node_t *remove_parent, *repalce_node;
  int is_remove_black, is_remove_left;

  if (p->left!=t->nil&&p->right!=t->nil)
  {
    remove = get_next_node(t,p);
    repalce_node = remove->right;
    p->key = remove->key;
  } else {
    remove = p;
    repalce_node = (remove->right != t->nil)? remove->right:remove->left;
  }
  remove_parent = remove->parent;

  if (remove == t->root)
  {
    t->root = repalce_node;
    t->root->color = RBTREE_BLACK;
    free(remove);
    return 0;
  }
  is_remove_black = remove->color;
  is_remove_left = remove_parent->left == remove;
  
  if (is_remove_left) remove_parent->left = repalce_node;
  else remove_parent->right = repalce_node;
  
  repalce_node->parent = remove_parent;
  free(remove);

  if (is_remove_black)
  {
    rbtree_erase_fixup(t, remove_parent, is_remove_left);
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n){
  node_t *current = rbtree_min(t);
  arr[0] = current->key;
  for (int i = 1; i < n; i++)
  {
    if (current == t->nil) break;
    current = get_next_node(t,current);
    if (current == t->nil) break;
    arr[i] = current->key;
  }
  return 0;
}
