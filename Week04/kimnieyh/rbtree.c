#include "rbtree.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


int pow_2(int x) {
  // 2**x 계산 함수
  return 1 << x;
}

void record_keys(rbtree *t, node_t *p, node_t **node_arr, int *node_idx_list, int cur_depth, int render_depth) {
  // visualize_tree에서 사용하기위한 배열을 재귀적으로 기록
  if (cur_depth > render_depth) {
    return;
  }
  
  node_arr[cur_depth * pow_2(render_depth) + node_idx_list[cur_depth]] = p;
  node_idx_list[cur_depth] += 1;
  if (p == t->nil) {
    record_keys(t, t->nil, node_arr, node_idx_list, cur_depth +1, render_depth);
    record_keys(t, t->nil, node_arr, node_idx_list, cur_depth +1, render_depth);
  } else {
    record_keys(t, p->left, node_arr, node_idx_list, cur_depth +1, render_depth);
    record_keys(t, p->right, node_arr, node_idx_list, cur_depth +1, render_depth);
  }
}

void visualize_tree(rbtree *t, int render_depth) {
  // 트리 시각화 함수
  // depth는 루트를 0으로 계산
  // key_arr[i][j]: 깊이가 i인 (j+1)번째 원소의 키 **2차원 배열 접근이 불가능해 1차원 배열로 구현
    node_t **node_arr = calloc( (size_t) ((render_depth +1) * pow_2(render_depth)), sizeof(node_t));

  // key_idx_list[i]: key_arr[i]의 현재 인덱스
  int *node_idx_list = (int *) calloc((size_t) (render_depth +1), sizeof(int));

  char *filler = "  ";

  // 키를 배열에 기록
  record_keys(t, t->root, node_arr, node_idx_list, 0, render_depth);

  // 기록된 키를 출력
  for (int i = 0; i < render_depth +1; i++) {
    for (int j = 0; j < pow_2(i); j++) {
      for (int k = 0; k < pow_2(render_depth - i); k++) { // front buffer
        printf("%s", filler);
      }
      if (node_arr[i * pow_2(render_depth) + j] == t->nil) {
        printf("\x1b[0m" "%2s", ".");
      } else if (node_arr[i * pow_2(render_depth) + j]->color == RBTREE_BLACK) {
        // 검정 노드는 흰색으로 출력
        printf("\x1b[0m" "%2d", node_arr[i * pow_2(render_depth) + j]->key);
      } else {
        // 빨강 노드는 빨간색으로 출력
        printf("\x1b[31m" "%2d", node_arr[i * pow_2(render_depth) + j]->key);
      }
      
      for (int k = 0; k < pow_2(render_depth - i) -1; k++) { // back buffer
        printf("%s", filler);
      }
    }
    printf("\n");
  }
  printf("\n");

  free(node_arr);
  free(node_idx_list);
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = calloc(1,sizeof(node_t));

  nil_node->color = RBTREE_BLACK;
  nil_node->key = 0;
  p->nil = nil_node;
  p->root = p->nil;
  return p;
}
node_t *new_node(rbtree *t, key_t key)
{
    node_t *n = calloc(1,sizeof(node_t));
    n->left = t->nil;
    n->right = t->nil;
    n->key = key;
    n->color = RBTREE_RED;
    return n;
}
void delete_node(node_t *n,rbtree *t){
  if(n == t->nil){
    return;
  }
  delete_node(n->left,t);
  delete_node(n->right,t);
  free(n);
  n = NULL;
  return;
}
void delete_rbtree(rbtree *t) {
  if(t == NULL){
    return;
  }
  delete_node(t->root,t);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
  return;
}
void left_rotation(rbtree *t,node_t *x)
{
    //x의 오른쪽 자식 노드 y 선언 
    node_t *y = x->right;
    //y의 왼쪽 자식노드를 x의 오른쪽 자식노드로 변경 
    x->right = y->left;
    //y의 자식노드가 NIL이 아니면, y의 왼쪽 자식노드의 부모를 x로 변경
    if (y->left != t->nil)
        y->left->parent = x;
    //y의 부모노드를 x의 부모로 변경
    y->parent = x->parent;
    //x is root
    if(x->parent == t->nil)
        t->root = y;
    // x is left child
    else if(x == x->parent->left)
        x->parent->left = y;
    else //x is right child
        x->parent->right = y;
    // y의 왼쪽 자식을 x로 
    y->left = x;
    // x의 부모를 y로 
    x->parent = y;
}
void right_rotation(rbtree *t,node_t *x)
{
    node_t *y = x->left;
    x->left = y->right;
    if(y->right != t->nil)
        y->right->parent = x;
    y->parent = x->parent;
    //x is root
    if(x->parent == t->nil)
        t->root = y;
    // x is left child 
    else if (x==x->parent->left)
        x->parent->left = y;
    else // x is right child
        x->parent->right = y;
    y->right = x;
    x->parent = y;
}
void insert_fixup(rbtree *t,node_t *z)
{
    while(z->parent->color == RBTREE_RED)
    {
        if (z->parent == z->parent->parent->left) // z.parent is left child
        {
            node_t *y = z->parent->parent->right; // uncle of z
            if (y->color == RBTREE_RED) // case 1 
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }else // case 2 or 3
            {
                if (z== z->parent->right) // case 2 
                {
                    z = z->parent; // marked z.parent as new z
                    left_rotation(t,z);
                }
                // case 3 
                z->parent->color = RBTREE_BLACK; //made parent black
                z->parent->parent->color = RBTREE_RED; // made grandparent red
                right_rotation(t,z->parent->parent);
            }
        }else // z.parent is right child
        {
            node_t *y = z->parent->parent->left; // uncle of z 
            if (y->color == RBTREE_RED) // case 1 
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }else
            {
                if (z==z->parent->left)
                {
                    z = z->parent;
                    right_rotation(t,z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotation(t,z->parent->parent);
            }
        }
    }
    // tree의 root를 black 으로
    t->root->color = RBTREE_BLACK;
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *y = t->nil;
    node_t *n = new_node(t,key);
    node_t *temp = t->root;

    while(temp != t->nil)
    {
        y = temp;
        if(n->key < temp->key)
            temp = temp->left;
        else
            temp = temp->right;
    } 
    n->parent = y;
    if(y == t->nil)
        t->root = n;
    else if (n->key < y->key)
        y->left = n;
    else
        y->right = n;
    n->left = t->nil;
    n->right = t->nil;
    n->color = RBTREE_RED;
    insert_fixup(t,n);
  return n;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;
  while (x!=t->nil && x->key != key){
    if(key>x->key)
        x = x->right;
    else if(key<x->key)
        x = x->left;
  }
  if (x == t->nil)
    x = NULL;
  return x;
}
node_t* minimum(rbtree *t, node_t *x){
    while(x->left != t->nil)
        x = x->left;
    return x;
}
node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  while(x->left != t->nil)
        x = x->left;
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  while(x->right != t->nil)
        x = x->right;
  return x;
}
void transplant(rbtree *t, node_t *u, node_t *v)
{
    if(u->parent == t->nil) // u is root
        t->root = v;
    else if(u == u->parent->left) // u is left child
        u->parent->left = v;
    else // u is right child
        u->parent->right = v;
    v->parent = u->parent;
}
void delete_fixup(rbtree *t, node_t *x)
{
    while (x!= t->root && x->color == RBTREE_BLACK)
    {
        if(x == x->parent->left)
        {
            node_t *w = x->parent->right;
            if(w->color == RBTREE_RED) // case 1
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotation(t,x->parent);
                w= x->parent->right;
            }
            if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)//case 2
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }else //case 3 or 4
            {
                if(w->right->color == RBTREE_BLACK) //case 3 
                {
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotation(t,w);
                    w = x->parent->right;
                }//case 4
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotation(t,x->parent);
                x= t->root;
            }
        }else{
            node_t *w = x->parent->left;
            if(w->color ==RBTREE_RED) //case 1 
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotation(t,x->parent);
                w = x->parent->left;
            }
            if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK ) //case 2 
            {
                w->color = RBTREE_RED;
                x= x->parent;
            }else //case 3 or 4
            {
                if(w->left->color == RBTREE_BLACK) //case 3
                {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotation(t,w);
                    w = x->parent->left;
                }//case 4
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                right_rotation(t,x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
}
int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  node_t *x;
  color_t y_origin_color = y->color;
  
  if(p->left == t->nil) // no children or only right
  {
      x = p->right;
      transplant(t,p,p->right);
  }else if (p->right == t->nil) // only left
  {
      x = p->left;
      transplant(t,p,p->left);
  }else { //both children
      y = minimum(t,p->right);
      y_origin_color = y->color;
      x = y->right;
      if (y->parent == p) // y가 z의 자식
      {
          x->parent = y;
      }else{
          transplant(t,y,y->right);
          y->right = p->right;
          y->right->parent = y;
      }
      transplant(t,p,y);
      y->left = p->left;
      y->left->parent = y;
      y->color = p->color;
  }
  if(y_origin_color == RBTREE_BLACK)
  {
      delete_fixup(t,x);
  }
  free(p);
  p = NULL;
  return 0;
}
int inorder_array(node_t *nil,node_t *root,key_t *arr,const size_t n, int index){
  if(root == nil){
    return index;
  }
  if(index == n){
    return index;
  }
  index = inorder_array(nil,root->left,arr,n,index);
  if(index<n){
    arr[index++] = root->key;
  }
  index= inorder_array(nil,root->right,arr,n,index);
  return index;
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if(t->root == t->nil){
    return 0;
  }
  inorder_array(t->nil,t->root,arr,n,0);
  return 0;
}