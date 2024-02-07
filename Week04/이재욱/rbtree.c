#include "rbtree.h"
#include<stdio.h>
#include <stdlib.h>



//트리 생성 
rbtree *new_rbtree(void) 
{
  // 트리 구조 동적할당을 해줘야함. 여러 노드들을 저장해야하기 떄문
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  //rb트리의 리프노드는 nil노드로 표시
  //nil노드 가상의 노드로 생각 리프노드들 할당
  node_t*nil = (node_t*)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  
  //트리가 비었을 때 nil노드가 할당되어야함.
  t->nil = t->root = nil;
  return t;
}
//트리를 돌면서 모든 노드의 메모리 반환 
//자식노드가 nil이 아닐시 자식 노드를 루트로 만들어 재귀적으로 함수를 호출
void delete_rbtree(rbtree *t) {
  
  node_t*node = t->root;
  if(node != t->nil)
    traverse_and_delete_node(t, node);

  //할당해제 메모리 반환
  free(t->nil);
  free(t);
}

//각각의 노드와 자식노드들을 반환 메모리 반환해야하니까
void traverse_and_delete_node(rbtree *t, node_t *node)
{
  if(node->left != t->nil)
  traverse_and_delete_node(t, node->left);
  if(node->right != t->nil)
  traverse_and_delete_node(t, node->right);
  
  //현재 노드의 메모리 반환
  free(node);
}

//새로운 노드를 동적할당이고 생성 키값은 변하면 안됨.
node_t *rbtree_insert(rbtree *t, const key_t key) {
  
  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->left = new_node->right = t->nil;

  node_t*current = t->root;
  while (current != t->nil)
  {
    if(key < current -> key)
    {
      if(current->left == t->nil)
      {
        current->left = new_node;
        break;
      }
      current = current->left;
    }
    else
    {
      if(current->right == t->nil)
      {
        current->right = new_node;
        break;
      }
      current = current->right;
    }
  }
  
  new_node->parent = current;

  if(current == t->nil)
  t->root = new_node;

  rbtree_insert_fixup(t, new_node);
  return new_node;
 
}


//rbtree의 case를 추가하는 함수
//case는 총 3개로 만들었음. 1. uncle이 red
//2. uncle이 black, parent와 new node가 left, right 자식일때
//3. uncle이 black, parent가 left, right, newnode가 right,left 일때의 케이스를 저장
void rbtree_insert_fixup(rbtree*t, node_t*node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle;
  //일단은 왼쪽부터 확인해봐야함
  int is_left = node == parent->left;
  //노드 기준으로 왼쪽 자식인지 확인
  int is_parent_is_left;
  //처음에 삽입시 레드로 들어가니까 블랙으로 바꿔줌(루트라서)
  if(node == t->root)
  {
    node->color = RBTREE_BLACK;
    return;
  }
  //parent가 black일때는 아무 변화 없음.
  if(parent->color == RBTREE_BLACK)
  return;
  is_parent_is_left = grand_parent->left == parent;
  uncle = (is_parent_is_left)?grand_parent->right:grand_parent->left;

  // case1 parent와 uncle이 모두 레드일때
  if(uncle->color == RBTREE_RED)
  {
    parent->color = RBTREE_BLACK;
    uncle->color = RBTREE_BLACK;
    grand_parent->color = RBTREE_RED;
    rbtree_insert_fixup(t, grand_parent);
    return;
  }
  if(is_parent_is_left)
  {
    //case2 uncle이 black 그리고 parents가 left 그리고 current 노드가 left에 있을 때
    if(is_left)
    {
      right_rotate(t, parent);
      exchange_color(parent, parent->right);
      return;
    }
    //case3
    left_rotate(t, node);
    right_rotate(t, node);
    exchange_color(node, node->right);
    return;
}
  if(is_left)
  {
    right_rotate(t, node);
    left_rotate(t, node);
    exchange_color(node, node->left);
    return;
  }

  left_rotate(t, parent);
  exchange_color(parent, parent->left);
}

//회전하는 함수 

void left_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_left = node->left;

  if(parent == t->root)
    t->root = node;
  else
  {
    if(grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent;
  parent->parent = node;
  node->left = parent;
  parent->right = node_left;
  node_left->parent = parent;
}

void right_rotate(rbtree*t, node_t*node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_right = node->right;

  if(parent == t->root)
  t->root = node;
  else
  {
    if(grand_parent->left == parent)
    grand_parent->left = node;
    else
    grand_parent->right = node;
  }
  node->parent = grand_parent;
  parent->parent = node;
  node->right = parent;
  node_right->parent=parent;
  parent->left = node_right;
}


node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  node_t *current = t->root;
  while(current != t->nil)
  {
    if(key == current->key)
    return current;
    else
      current =(key < current -> key)?current->left:current->right;
  }  
  return NULL;
}

//key의 최소값을 반환하는 함수
node_t *rbtree_min(const rbtree *t) {
  node_t *current = t->root;
  while(current->left != t->nil)
  current = current->left;
  return current;
}
//최대값을 반환하는 함수
node_t *rbtree_max(const rbtree *t) {
  node_t*current = t->root;
  while (current->right!=t->nil)
  current = current->right;
  return current;
  
}
//중위탐색으로 순회한 노드를 배열에 담는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *currnet =rbtree_min(t);
  arr[0] = currnet -> key;

  for (int i = 1; i < n; i++)
  {
    if(currnet == t -> nil)
      break;
    currnet = get_node(t, currnet);
    if(currnet == t -> nil)
       break;
    arr[i] = currnet->key;
  }
  return 0;
}

int rbtree_erase(rbtree *t, node_t *delete) {
  node_t *remove;
  node_t *remove_parent, *replace_node;
  int is_remove_black, is_remove_left;

  if(delete->left != t->nil && delete->right != t->nil)
  {
    remove = get_node(t, delete);
    replace_node = remove->right;
    delete->key = remove->key;
  }
  else
  {
    remove = delete;
    replace_node = (remove->right != t->nil) ? remove->right : remove->left;
  }
  remove_parent = remove->parent;

  if (remove == t->root)
  {
    t->root = replace_node;
    t->root->color = RBTREE_BLACK;
    free(remove);
    return 0;
  }

  is_remove_black = remove->color;
  is_remove_left = remove_parent->left == remove;
  
  if(is_remove_left)
    remove_parent->left = replace_node;
  else
    remove_parent->right = replace_node;
  
  replace_node->parent = remove_parent;
  free(remove);

  if(is_remove_black)
    rbtree_erase_fixup(t, remove_parent, is_remove_left);
  return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left)
{
  node_t * extra_black = is_left ? parent->left : parent->right;
  if(extra_black->color == RBTREE_RED)
  {
    extra_black->color = RBTREE_BLACK;
    return;
  }

  node_t *sibling = is_left ? parent->right : parent -> left;
  node_t *sibling_left = sibling->left;
  node_t *sibling_right = sibling->right;

  if(sibling->color == RBTREE_RED)
  {
    if(is_left)
      left_rotate(t, sibling);
    else
      right_rotate(t, sibling);
    exchange_color(sibling, parent);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  node_t *near = is_left ? sibling_left : sibling_right;
  node_t *distance = is_left ? sibling_right : sibling_left;

  if(is_left && near->color == RBTREE_RED && distance->color == RBTREE_BLACK)
  {
    right_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }
  if(is_left && distance->color == RBTREE_RED)
  {
    left_rotate(t, sibling);
    exchange_color(sibling, parent);
    distance->color = RBTREE_BLACK;
    return;
   }
   if(near->color == RBTREE_RED && distance->color == RBTREE_BLACK)
   {
    left_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
   }
   if(distance -> color == RBTREE_RED)
   {
    right_rotate(t, sibling);
    exchange_color(sibling, parent);
    distance -> color = RBTREE_BLACK;
    return;
   }
   sibling -> color =RBTREE_RED;

   if(parent != t -> root)
    rbtree_erase_fixup(t, parent -> parent, parent -> parent -> left == parent);
}
//a,b노드의 색을 바꾸는 로직 
//만약 tmp의 색이 black이라면 b의 색도 black이 되고 그렇지 않으면 red가 된다.
void exchange_color(node_t *a, node_t *b)
{
  int tmp = a -> color;
  a->color = b -> color;
  b->color = (tmp == RBTREE_BLACK) ? RBTREE_BLACK:RBTREE_RED;
}

//키 값을 기준으로 다음 노드를 반환
node_t *get_node(const rbtree *t, node_t *p)
{
  //right에 자식이 없다는 조건
  node_t *current = p -> right;
  if(current == t -> nil)
  {
    current = p;
    while (1)
    {
      //current가 오른쪽 자식일 때, 부모 노드로 이동
      if(current -> parent -> right == current)
        current = current -> parent;
      else
        return current -> parent;
    }    
  }
  //왼쪽에 자식이 있을 때는 
  //왼쪽 끝으로 이동함.
    while(current -> left != t -> nil)
      current = current ->left;
    return current;
}
