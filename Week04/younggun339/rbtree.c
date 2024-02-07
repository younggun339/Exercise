#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  // rbtree 크기만큼 증가할때마다 1씩 할당.  
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // 여기서 nil도 일종의 노드 처럼 할당해서 검정 속성 부여.
  // 그냥 Null을 쓰는 대신 검은색 가짐!
  if (p != NULL){
    p->nil= (node_t*)calloc(1,sizeof(node_t));
    p->nil->color = RBTREE_BLACK;
    p->root = p->nil;
    // dangler poiner 로 해봤으나 먹히지 않았다..
    // p->nil->key = 0;
  
    return p;
  }
  else{
    // 그냥 별도로 True, False 0, 1로 헤더에서 설정해놔서 그렇다.
    return FALSE;
  }

}

// 왼쪽으로 회전.
//(x의 오른쪽 자식 노드 y가 x자리에,)
//(x는 y의 왼쪽 자식이 된다.)
void left_rotate(rbtree *t, node_t *x){

  node_t *y;
  
  // y가 x의 오른쪽 자식일때.
  y = x->right;

  if(y != t->nil){
    // (손자 정리 시즌)
    // x의 오른쪽에는, 오른쪽 자식y의 왼쪽 자식(손자) 트리를 붙인다.
    x->right = y->left;

    // 근데 y의 왼쪽 자식이 nil이 아니라면 그 노드의 부모도 설정해준다.
    if (y->left != t->nil){
        y->left->parent = x;
    }

    // (y 이동 시즌 )
    // 그리고 y도 자리를 옮겨, y도 부모를 설정해준다.
    y->parent = x->parent;

    // 만약 y가 최고 조상이었으면, root임을 알려준다.
    if (x->parent == t->nil){
        t->root = y;
    }
    // 조상의 왼쪽 자식이었으면, y도 왼쪽 자식.
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    // 조상의 오른쪽 자식이었으면, y도 오른쪽 자식.
    else {
        x->parent->right = y;
    }
    // (x 이동 시즌)
    // y의 자식으로 x를, x부모에도 y를 설정.
    y->left = x;
    x->parent = y;}

}


// 오른쪽으로 회전.
// x는 부모 y의 왼쪽 자식인데
// x가 부모 y의 자리에 오고, y는 x의 오른쪽 자식이 된다.
void right_rotate(rbtree *t, node_t *y){

  node_t *x;

  x = y->left;
  if(x != t->nil){
    y->left = x->right;

    if (x->right != t->nil){
        x->right->parent = y;
    }

    x->parent = y->parent;

    if(y->parent == t->nil){
        t->root = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else{
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
  }

}

// 중위 순회식으로 노드 지우기
void inorder_tree_free(rbtree *t, node_t *x){
    

    if(x!=t->nil){
  
        inorder_tree_free(t, x->left);
        inorder_tree_free(t,x->right);
        free(x);
    }
  
}

// 트리 지우기
void delete_rbtree(rbtree *t) {

  node_t *y= t->root;

  inorder_tree_free(t, y);

  free(t->nil);
  free(t);
  t = NULL;
}

//삽입 시 속성 위반
void *rbtree_insert_fixup(rbtree *t, node_t *z) {
    node_t *y;

    // 현재 부모가 레드라면
    while (z->parent->color == RBTREE_RED) {
        // 부모가 할아버지의 왼쪽 자식일때
        if (z->parent == z->parent->parent->left) {
            // y는 삼촌이라고 할 때
            y = z->parent->parent->right;

            // 삼촌도 레드면
            if (y->color == RBTREE_RED) {
                // 부모 블랙! 삼촌 블랙! 할아버지 레드!
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                // z를 할아버지로 설정!
                z = z->parent->parent;
            } else {
                // 현재 애가 부모의 오른쪽 자식이면(꺾인 상태면)
                if (z == z->parent->right) {
                    // 부모 기준으로 왼쪽 회전!
                    z = z->parent;
                    left_rotate(t, z);
                }
                // 현재 펴진 상태.
                // 부모 색깔 블랙! 할아버지 색깔 레드! 할아버지 기준 오른쪽 회전!
                z->parent->color = RBTREE_BLACK;
                if (z->parent != t->nil && z->parent->parent != t->nil) {
                    z->parent->parent->color = RBTREE_RED;
                    right_rotate(t, z->parent->parent);
                }
            }
        } else if (z->parent == z->parent->parent->right) {
            // 부모가 할아버지의 오른쪽 자식일 때
            y = z->parent->parent->left;

            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                if (z->parent != t->nil && z->parent->parent != t->nil) {
                    z->parent->parent->color = RBTREE_RED;
                    left_rotate(t, z->parent->parent);
                }
            }
        }
    }

    t->root->color = RBTREE_BLACK;

    return 0;
}


// rb트리 삽입.
 node_t *rbtree_insert(rbtree *t, const key_t key) {

   node_t *y = t->nil;
   node_t *x = t->root;
   node_t *z = (node_t *)malloc(sizeof(node_t));
   
  
// 넣을 노드는 빨간색. 노드 형에 값을 넣어줘서 z노드를 만든다.
   z->key = key;
   z->color = RBTREE_RED;
   z->left = t->nil;
   z->right = t->nil;
   z->parent = t->nil;

// 근데 루트가 안 비어있다면,
   while (x != t->nil)
   {
    // y로 현재 지점은 저장하고, 맞는 위치까지, nil을 만날 때까지 트리 아래로 하강.
     y = x;
     if(z->key < x->key){
       x = x->left;
     }
     else{
       x = x->right;
     }
   }

  // 찾았을테니, 현재 지점을 z의 부모로 설정하는데.... 
   z->parent = y;

  // 만약 그 자리가 최고 조상이면 z가 루트.
   if (y == t->nil){
     t->root = z;
   }
  //크기에 따라 z가 왼쪽 자식인지 오른쪽 자식인지 지정.
   else if (z->key < y->key)
   {
     y->left = z;
   }
   else{
     y->right = z;
   }
   // 잘 했으니 z에게 nil 들을 붙여주고, z 색도 다시 빨강!
   z->left = t->nil;
   z->right = t->nil;
   z->color = RBTREE_RED;
  rbtree_insert_fixup(t,z);

  return t->root;
 }


// rb트리에서 값을 찾는 함수. 
 node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;
  node_t *y;
  while (x != t->nil)
  {
    if(key < x->key){
      y = x;
      x = x->left;
    }
    else if(key > x->key){
      y = x;
      x = x->right;
    }
    else{
      return x;
    } 
  }
  return NULL;

 }

// rb트리에서 가장 작은 값을 찾는 함수.
 node_t *rbtree_min(const rbtree *t) {

  node_t *x = t->root;
  node_t *z;

  z = x;

  while(x != t->nil){
    z = x;
    x = x->left;
  }

  return z;
 }


// rb트리에서 가장 큰 값을 찾는 함수.
 node_t *rbtree_max(const rbtree *t) {

  node_t *x = t->root;
  node_t *z;

  z = x;

  while (x != t->nil)
  {
    z = x;
    x = x->right;
  }
  
  return z;
 }

// rb트리에서 두 노드의 자리를 바꾸는 함수.
// 정확히는 u자리에 v만 들어갈뿐임.(v자리는 아무도 안들어감.)
void rb_transplant(rbtree *t, node_t *u, node_t *v){

// 루트면 본인.
  if (u->parent == t->nil){
    t->root = v;
  }
  // 어느 자식이었는지에 따라 설정.
  else if ( u == u->parent->left)
  {
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  // 부모 통일.
  v->parent = u->parent;
  
}
// 삭제시 위반 속성을 검사하는 함수.
 void rbtree_delete_fixup(rbtree *t, node_t *x){

  node_t *w;

// x가 루트가 아니고 삭제색깔이 검정이라면.
  while (x != t->root && x->color== RBTREE_BLACK){
    // x가 부모의 왼쪽 자식 일 경우.
    if(x == x->parent->left){
      w = x->parent->right;
      // 형제가 RED
      if(w->color == RBTREE_RED){
        w->color =RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t,x->parent);
        w = x->parent->right;
      }

      // 형제, 형제의 자식 둘다 블랙.
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // 형제의 꺾인 자식이 RED
      else { 
        
        if (w->right->color == RBTREE_BLACK){
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            right_rotate(t, w);
            w = x->parent->right;
        }
        // 형제의 펴진 자식이 RED
        else{
            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->right->color = RBTREE_BLACK;
            left_rotate(t,x->parent);
            x = t->root;
        }
      }
    }

    // x가 부모의 오른쪽 자식일 경우.
    else{
      w = x->parent->left;
      // 형제가 RED
      if(w->color == RBTREE_RED){
        w->color =RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left;
      }

      // 형제, 형제 자식 둘 다 블랙.
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // 형제 꺾인 자식 RED
      else{ 
        
        if (w->left->color == RBTREE_BLACK){
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotate(t, w);
            w = x->parent->left;
        }
        // 형제 펴진 자식 RED
        else{
            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->left->color = RBTREE_BLACK;
            right_rotate(t,x->parent);
            x = t->root;
        }
      }
    }

  }
  x->color = RBTREE_BLACK;
 }


 node_t *tree_minimum(rbtree *t,  node_t *x){
  
  while(x->left != t->nil){
    x = x->left;
  }

  return x;
 }


 int rbtree_erase(rbtree *t, node_t *z) {

  node_t *y;
  node_t *x;

  color_t succesor;

  y = z;
  succesor = y->color;

  if(z->left == t->nil){
    x = z->right;
    rb_transplant(t,z,z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rb_transplant(t,z,z->left);
  }
  else{
    y = tree_minimum(t,z->right);
    succesor = y->color;
 
    x = y->right;
    if(y->parent == z){
      x->parent = y;
    }
    else{
      rb_transplant(t,y,y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rb_transplant(t,z,y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  
  if(succesor == RBTREE_BLACK){
    rbtree_delete_fixup(t,x);
  }
  
  if(z == z->parent->left){
    z->parent->left = t->nil;
  }
  else if(z == z->parent->right){
    z->parent->right = t->nil;
  }

  free(z);
  z = t->nil;
  
  return 0;
 }

 void array_rbtree(const rbtree *t, key_t *arr, node_t *root, int *index_x){

    if(root != t->nil){
        array_rbtree(t, arr, root->left, index_x);

        arr[*index_x] = root->key;
        (*index_x)++;

        array_rbtree(t, arr, root->right, index_x);
    }

 }

 int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {

 if(arr != NULL && t != NULL){
    int index_x = 0;
    array_rbtree(t, arr, t->root, &index_x);
 }
 
 return 0;
 }

