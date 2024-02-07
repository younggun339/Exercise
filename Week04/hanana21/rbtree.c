#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
    rbtree *t = (rbtree*)calloc(1,sizeof(rbtree));
    node_t *nil =(node_t*)calloc(1,sizeof(node_t));
    nil->color = RBTREE_BLACK;
    t->nil = t->root = nil; //빈트리일때 root는 nil노드여야함
    return t;
}

void delete_rbtree(rbtree *t) {
    node_t* node = t->root;
    if (node != t->nil){
        traverse_and_delete_node(t,node);
    }
    free(t->nil);
    free(t);
}
void traverse_and_delete_node(rbtree* t, node_t* node) {
    if (node->left != t->nil) {
        traverse_and_delete_node(t, node->left);
    }
    if (node->right != t->nil) {
        traverse_and_delete_node(t, node->right);
    }
    free(node);
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
    //새 노드 만들기
    node_t* new_node = (node_t*)calloc(1,sizeof(node_t));
    new_node -> key = key;
    new_node -> color = RBTREE_RED;
    new_node -> left = new_node ->right = t->nil;
    //노드 삽입 할 위치 찾기 
    node_t* current = t->root;
    while (current != t->nil){
        if(key<current->key){
            if(current->left == t->nil){ //자식 없으면 걍 넣기
                current->left = new_node;
                break;
            }
            current = current->left; 
        }else{
             if(current->right == t->nil){
                current->right = new_node;
                break;
            }
            current = current->right;
        }
    }
    new_node->parent = current;
    //빈트리라면 루트노드로 설정
    if (current == t->nil){
        t->root = new_node;
    }
    rbtree_insert_fixup(t,new_node);
    
    return new_node;
}
//색깔 바꾸기 
void exchange_color(node_t* a, node_t* b){
    int tmp = a->color;
    a->color = b->color;
//     b->color = (tmp ==RBTREE_BLACK)? RBTREE_BLACK:RBTREE_RED;
    b->color = tmp;
 }
// 삽입 노드 불균형 해결
void rbtree_insert_fixup(rbtree* t, node_t* Node){
    node_t* parent = Node->parent;
    node_t* grand_parent = parent -> parent;
    node_t* uncle;
    int is_left = Node ==parent->left; // 현재노드 왼쪽 오른쪽 확인
    int is_parent_is_left; // 부모 왼쪽 오른쪽 확인

    //추가된 노드가 root노드인 경우: 색만 변경
    if(Node == t->root){
        Node -> color = RBTREE_BLACK;
        return;
    }
    //부모가 black인 경우: 변경 없음 
    if(parent->color==RBTREE_BLACK){
        return;
    }
    is_parent_is_left = grand_parent->left == parent; //부모가 왼쪽 오른쪽 판단
    uncle =(is_parent_is_left)?grand_parent->right:grand_parent->left; // 부모위치에 따라 삼촌 위치 판단
    //부모와 부모의 형제가 RED인 경우
    if(uncle->color == RBTREE_RED){
        parent ->color = RBTREE_BLACK;
        uncle -> color = RBTREE_BLACK;
        grand_parent -> color = RBTREE_RED;
        rbtree_insert_fixup(t,grand_parent);
        return;
    }
    if(is_parent_is_left){
        //부모형제가 black, 부모가 왼쪽 자식, 현재노드가 왼쪽 자식
        if(is_left){
            right_rotate(t,parent);
            exchange_color(parent,parent->right);
            return;
        }
        //부모의 형제가 black, 부모가 왼쪽 자식, 현재 노드가 오른쪽 자식
        left_rotate(t,Node);
        right_rotate(t,Node);
        exchange_color(Node,Node->right);
        return;
    }
    //삼촌 black,부모가 오른쪽, 현재노드 왼쪽 
    if(is_left){
        right_rotate(t,Node);
        left_rotate(t,Node);
        exchange_color(Node,Node->left);
        return;
    }
    //삼촌 black,부모가 오른쪽, 현재노드 오른쪽 
    left_rotate(t,parent);
    exchange_color(parent,parent->left);
}
//오른쪽 회전
void right_rotate(rbtree* t, node_t* Node){
    node_t* parent = Node -> parent;
    node_t* grand_parent = parent->parent;
    node_t* node_right = Node->right;

    if(parent == t->root){
        t->root = Node;
    }else{
        if(grand_parent->left == parent){
            grand_parent -> left = Node;
        }else{
            grand_parent -> right =Node;
        }
    }
    Node -> parent = grand_parent;
    parent -> parent = Node;
    Node-> right = parent;
    node_right -> parent = parent;
    parent -> left = node_right;
}

//왼쪽으로 회전 
void left_rotate(rbtree* t, node_t* Node){
    node_t* parent = Node -> parent;
    node_t* grand_parent = parent->parent;
    node_t* node_left = Node->left;

    if (parent == t->root){
        t->root = Node;
    }else{
        if(grand_parent->left == parent){
            grand_parent->left = Node;
        }else{
            grand_parent->right = Node;
        }
    }
    Node -> parent = grand_parent;
    parent -> parent = Node;
    Node-> left = parent;
    parent -> right = node_left;
    node_left ->parent = parent;
}
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current  = t->root;
  while(current != t->nil){
    if(key == current -> key){
      return current;
    }else{
      current = (key < current->key)? current->left:current->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t* current = t-> root;
  while(current->left != t->nil){
    current = current->left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  node_t* current = t-> root;
  while(current->right != t->nil){
    current = current->right;
  }
  return current;
}

int rbtree_erase(rbtree *t, node_t *delete) {
  node_t* remove;//지워질 노드 
  node_t* remove_parent, *replace_node;
  int is_remove_black, is_remove_left;

  //자식이 2개인 경우 대체 노드 찾기
  if(delete->left != t->nil && delete->right != t->nil){
    remove = get_next_node(t,delete);
    replace_node = remove->right;//섹세서 자식
    delete -> key = remove -> key;
  }else{//자식이 1개인 경우
    remove = delete;
    replace_node = (remove -> right != t->nil)? remove-> right:remove->left;
  }
  remove_parent = remove->parent;
  //#1 루트 노드 제거하기
  if(remove == t->root){
    t->root = replace_node;
    t->root->color = RBTREE_BLACK;
    free(remove);
    return 0;
  }
  //remove의 부모와 자식 이어주기 
  is_remove_black = remove->color;//제거전 자기 색 저장
  is_remove_left = remove_parent->left==remove;

  if(is_remove_left){// 왼쪽 자식이었으면 왼쪽으로 이어주고
    remove_parent->left = replace_node;
  }else{//오른쪽 자식이었으면 오른쪽으로 이어준다.
    remove_parent->right = replace_node;
  }
  //부모도 연결(양방향)
  replace_node->parent = remove_parent;
  free(remove);

  //검정 노드 제거시 불균형 복구필요
  if(is_remove_black){
    rbtree_erase_fixup(t,remove_parent,is_remove_left);
  }
  return 0;
}
void rbtree_erase_fixup(rbtree*t, node_t* parent, int is_left){
  //삭제 후 대체 노드가 RED라면 그냥 BLACK으로 변경
  node_t* extra_black = is_left?parent->left:parent->right;
  if(extra_black->color==RBTREE_RED){
    extra_black->color = RBTREE_BLACK;
    return;
  }
  node_t* sibling = is_left?parent->right:parent->left;
  node_t* sibling_left = sibling->left;
  node_t* sibling_righ = sibling->right;
  //형제 색 빨간색인 경우
  if(sibling->color == RBTREE_RED){
    if(is_left){
      left_rotate(t,sibling);
    }else{
      right_rotate(t,sibling);
    }
    exchange_color(sibling,parent);
    rbtree_erase_fixup(t,parent,is_left);
    return;
  }
  node_t* near = is_left?sibling_left:sibling_righ;
  node_t* distant = is_left?sibling_righ:sibling_left;
  //부모는 왼쪽,형제가 black 형제의 가까운 자식 red, 먼자식black
  if(is_left && near->color ==RBTREE_RED && distant->color == RBTREE_BLACK){
    right_rotate(t,near);
    exchange_color(sibling,near);
    rbtree_erase_fixup(t,parent,is_left);
    return;
  }
  //부모는 왼쪽,형제가 black, 더 먼 자식 RED(펴진 애)
  if(is_left && distant->color == RBTREE_RED){
    left_rotate(t,sibling);
    exchange_color(sibling,parent);
    distant->color = RBTREE_BLACK;
    return;
  }
  //부모는 오른쪽, 형제가 블랙, 가까운 자식 레드, 형제 더먼 자식 black
  if(near->color ==RBTREE_RED && distant->color == RBTREE_BLACK){
    left_rotate(t,near);
    exchange_color(sibling,near);
    rbtree_erase_fixup(t,parent,is_left);
    return;
  }
  if(distant->color == RBTREE_RED){
    right_rotate(t,sibling);
    exchange_color(sibling,parent);
    distant->color = RBTREE_BLACK;
    return;
  }
  //형제가 블랙, 형제 자식 둘다 블랙
  sibling-> color = RBTREE_RED;

  if(parent != t->root)
    rbtree_erase_fixup(t,parent->parent,parent->parent->left==parent);
}

//t를 n번 순회한 결과를 arr에 담는 함수 
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t* current = rbtree_min(t);
  arr[0] = current->key;
  for(int i=1; i<n; i++){
    if(current == t->nil)
      break;
    current = get_next_node(t,current);
    if(current == t->nil)
      break;
    arr[i] = current->key;
  }
  return 0;
}
node_t* get_next_node(const rbtree* t, node_t *p){
  node_t* current = p->right;
  if(current == t->nil){//오른쪽 자식이 없다면
    current = p;
    while(1){
      if(current->parent->right == current){
        current = current->parent;
      }else{
        return current->parent;
      }
    }
  }  
  while(current->left != t->nil){
      current = current->left;
    }
  return current;
}
