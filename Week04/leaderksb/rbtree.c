#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)malloc(sizeof(node_t)); // nil노드 생성
  nil->color = RBTREE_BLACK;
  p->nil = p->root = nil;
  p->root->right = p->root->left = p->root->parent = nil; // root의 자식, 부모 nil로
  return p;
}
/* 수정할 RBtree와 서브트리의 루트(x)를 받아옴 */
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;  // y를 설정한다. y(돌릴 대상의 오른쪽 자식)는 x(돌릴 대상)의 오른쪽 자식 노드를 가리킵니다.
  x->right = y->left;  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮깁니다. 즉, y의 왼쪽 자식이 x의 오른쪽 자식이 됩니다.
  if (y->left != t->nil)
    y->left->parent = x;  // 만약 y의 왼쪽 자식이 nil이 아니라면, 그 부모를 x로 설정합니다.
  y->parent = x->parent;  // x의 부모를 y의 부모로 설정합니다. 즉, y는 x가 있던 위치로 옮겨집니다.
  if (x->parent == t->nil)
    t->root = y;  // 만약 x가 root 노드였다면, 이제 y가 새로운 root 노드가 됩니다.
  else if (x == x->parent->left)
    x->parent->left = y;  // 만약 x가 부모 노드의 왼쪽 자식이었다면, 이제 y가 그 위치를 차지합니다.
  else
    x->parent->right = y;  // 만약 x가 부모 노드의 오른쪽 자식이었다면, 이제 y가 그 위치를 차지합니다.
  y->left = x;  // y의 왼쪽 자식으로 x를 설정합니다.
  x->parent = y;  // 마지막으로, x의 부모를 y로 설정합니다.
}
// left_rotate에서 left, right 자리만 바꿔서 만듦
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil)
    y->right->parent = x;
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  y->right = x;
  x->parent = y;
}
void post_order_delete(rbtree *t, node_t *node) {  // 후위순회로 노드들 지워줌
  if (node != t->nil){
    post_order_delete(t, node->left);
    post_order_delete(t, node->right);
    free(node);
    node = NULL;  // 포인터 NULL로 설정
  }
}
void delete_rbtree(rbtree *t) {
  post_order_delete(t, t->root);  // 딸려 있는 노드 전체 free
  free(t->nil);  // nil노드 free
  t->nil = NULL;
  free(t);  // 트리 free
  t = NULL;
}
void rbtree_insert_fixup(rbtree *t, node_t *fix_node) {
  node_t *uncle; // 삼촌
  while (fix_node->parent->color == RBTREE_RED) {  // 항상 더블 레드
    if (fix_node->parent == fix_node->parent->parent->left) {  // 부모 방향이 어디서 오는지 확인
      uncle = fix_node->parent->parent->right;
      if (uncle->color == RBTREE_RED) {
        fix_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        fix_node->parent->parent->color = RBTREE_RED;
        fix_node = fix_node->parent->parent;
      }
      else {
        if (fix_node == fix_node->parent->right) {
          fix_node = fix_node->parent;  //돌리는 기준점 설정
          left_rotate(t, fix_node);
        }
        // 일자 상태
        fix_node->parent->color = RBTREE_BLACK;
        fix_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, fix_node->parent->parent);
      }
    }
    else { // 반대 방향
      uncle = fix_node->parent->parent->left;
      if (uncle->color == RBTREE_RED) {
        fix_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        fix_node->parent->parent->color = RBTREE_RED;
        fix_node = fix_node->parent->parent;
      }
      else {
        if (fix_node == fix_node->parent->left) {
          fix_node = fix_node->parent;
          right_rotate(t, fix_node);
        }
        fix_node->parent->color = RBTREE_BLACK;
        fix_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, fix_node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *new_node = (node_t *)malloc(sizeof(node_t));  // 삽입할 노드 만들어줌
  node_t *parent_node = t->nil;  // 삽입될 노드의 부모
  node_t *root_node = t->root;  // 루트
  new_node->key = key;
  while (root_node != t->nil){
    parent_node = root_node;
    if (new_node->key < root_node->key)
      root_node = root_node->left;
    else
      root_node = root_node->right;
  }
  new_node->parent = parent_node;  // 부모 관계 설정
  if (parent_node == t->nil)
    t->root = new_node;
  else if (new_node->key < parent_node->key)
    parent_node->left = new_node;
  else
    parent_node->right = new_node;
  new_node->color = RBTREE_RED;
  new_node->left = new_node->right = t->nil;
  rbtree_insert_fixup(t, new_node);
  return new_node;  // 삽입된 노드의 포인터
}
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;  // 찾은 노드를 가리키는 포인터
  while (x != t->nil && x->key != key) {
    if (x->key > key)
      x = x->left;
    else
      x = x->right;
  }
  if (x == t->nil)  // 찾고자하는 key가 없는 경우
    return NULL;
  return x;
}
node_t *rbtree_min(const rbtree *t) {
  node_t *p = t->nil;  // x의 부모
  node_t *x = t->root;
  while (x != t->nil) {
    p = x;
    x = x->left;
  }
  return p;
}
// root 기준 서브 트리에서의 최소 노드 포인터 리턴(successor 구하기 위해 구현)
node_t *subtree_min(rbtree *t, node_t *root) {
  node_t *p = root->parent;
  while (root != t->nil) {
    p = root;
    root = root->left;
  }
  return p;
}
node_t *rbtree_max(const rbtree *t) {
  node_t *p = t->nil;  // x의 부모
  node_t *x = t->root;
  while (x != t->nil) {
    p = x;
    x = x->right;
  }
  return p;
}
// u의 부모와의 관계를 v로 옮겨줌(자식은 건드리지 않음)
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil)  // u가 root였다면
    t->root = v;
  else if (u == u->parent->left)  // u가 왼쪽 자식이면
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}
void rbtree_delete_fixup(rbtree *t, node_t *x) {
  //x는 extra black
  node_t *sibling = NULL;  // x의 형제 sibling
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      sibling = x->parent->right;
      // 경우 1. 형제가 붉은 색
      if (sibling->color == RBTREE_RED) { 
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        sibling = x->parent->right;
      }
      // 경우 2. 형제가 모두 블랙 & 자녀가 모두 블랙
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        x = x->parent;
      }
      // 경우 3.
      else { 
        if (sibling->right->color == RBTREE_BLACK) {
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t, sibling);
          sibling = x->parent->right;
        }
        // 경우 4.
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    // x가 부모의 오른쪽 자식이면
    else {
      sibling = x->parent->left;
      // 경우 1. 형제가 붉은 색
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        sibling = x->parent->left;
      }
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        if (sibling->left->color == RBTREE_BLACK) {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = x->parent->left;
        }
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  // 루트는 항상 블랙
  x->color = RBTREE_BLACK;
}
// 삭제할 노드의 포인터를 받아서 rb트리 구조를 유지하며 해당 키 삭제
// 자신을 가리키는 부모가 없다면 그 노드를 free시켜줘야 한다
int rbtree_erase(rbtree *t, node_t *delete_node) {
  // TODO: implement erase
  node_t *y = delete_node;
  node_t *x = NULL;  // 삭제된 노드의 위치(extra black을 붙이기 위한)를 가리킴
  color_t y_original_color = y->color;
  if (delete_node->left == t->nil) {  // 삭제할 노드의 왼쪽 자식이 없는 경우, 둘다 없는 경우
    x = delete_node->right;
    rbtree_transplant(t, delete_node, delete_node->right);  // 삭제될 노드의 오른쪽 자식을 삭제될 노드 자리에 넣어줌 -> delete_node 없어짐
  }
  else if (delete_node->right == t->nil) {  // 삭제할 노드의 오른쪽 자식이 없는 경우
    x = delete_node->left;
    rbtree_transplant(t, delete_node, delete_node->left);
  }
  // 자식이 둘 다 있는 경우
  else {
    y = subtree_min(t, delete_node->right);  // successor
    y_original_color = y->color;
    x = y->right;
    if (y->parent == delete_node)  // delete_node의 succ이 delete_node의 바로 오른쪽 자식이었던 경우
      x->parent = y;
    else {
      rbtree_transplant(t, y, y->right);  // y(succ)의 오른쪽 자식이 y의 자리를 차지(succ은 올라가기 때문에)
      y->right = delete_node->right;  // succ(y)을 지우는 노드 위치로 올려주기
      y->right->parent = y;
    }
    rbtree_transplant(t, delete_node, y);
    y->left = delete_node->left;  // succ을 삭제할 delete_node자리로 완전히 옮겨줌
    y->left->parent = y;
    y->color = delete_node->color;
  }
  if (y_original_color == RBTREE_BLACK)  // 실질적으로 삭제된 노드가 검정색이었다면 고쳐줘야함
    rbtree_delete_fixup(t, x);
  free(delete_node);  // 위 코드를 보면 delete_node가 전부 transplant에 의해 부모를 잃었음(정확히는 부모가 delete_node를 가리키지 않음)
  delete_node = NULL;
  return 0;
}
void in_order(const rbtree *t, node_t *node, key_t *arr, int *i) {
  if (node != t->nil){
    in_order(t, node->left, arr, i);
    arr[*i] = node->key;
    (*i)++;
    in_order(t, node->right, arr, i);
  }
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int i  = 0;
  in_order(t, t->root, arr, &i);
  return 0;
}
