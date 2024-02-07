#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

void traverse_and_delete_node(rbtree *t, node_t *node);
void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
node_t *get_next_node(const rbtree *t, node_t *p);
void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left);
void exchange_color(node_t *a, node_t *b);


// 새 트리를 생성하는 함수
rbtree *new_rbtree(void)
{
  // tree 구조체 동적 할당
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 노드 생성 및 초기화
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK; // nil 노드는 항상 BLACK

  // tree의 nil과 root를 nil 노드로 설정 (tree가 빈 경우 root는 nil노드여야 한다.)
  t->nil = t->root = nil;

  return t;
}

//RB tree 구조체가 차지했던 메모리 반환

void delete_rbtree(rbtree *t)
{
  node_t *node = t->root;
  if (node != t->nil)
    traverse_and_delete_node(t, node);

  // nil 노드와 rbtree 구조체의 메모리를 반환
  free(t->nil);
  free(t);
}

//각 노드와 그 자식 노드들의 메모리를 반환하는 함수, 후위 순회를 사용하여 모든 노드를 방문하고 삭제하는 함수
void traverse_and_delete_node(rbtree *t, node_t *node)
{
  if (node->left != t->nil)
    traverse_and_delete_node(t, node->left);
  if (node->right != t->nil)
    traverse_and_delete_node(t, node->right);
  // 현재 노드의 메모리를 반환
  free(node);
}

//노드 삽입
//노드를 삽입하고 불균형을 잡는 함수
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  //새 노드를 생성
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t)); //new_node 재정의
  new_node->key = key;
  new_node->color = RBTREE_RED;              // 항상 레드로 추가
  new_node->left = new_node->right = t->nil; // 추가한 노드의 자식들을 nil 노드로 설정

  //새 노드를 삽입할 위치 탐색
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key < current->key) //넣는 데이터보다 current 데이터가 더 크면
    {
      if (current->left == t->nil)
      {
        current->left = new_node; // 새 노드를 왼쪽 자식으로 추가
        break;
      }
      current = current->left;
    }
    else
    {
      if (current->right == t->nil)
      {
        current->right = new_node; // 새 노드를 오른쪽 자식으로 추가
        break;
      }
      current = current->right;
    }
  }

  new_node->parent = current; // 새 노드의 부모 지정

  // root가 nil이면(트리가 비어있으면) 새 노드를 트리의 루트로 지정
  if (current == t->nil)
    t->root = new_node;

  // 불균형 복구
  rbtree_insert_fixup(t, new_node);
  //불균형이 발생하지 않는다면 rbtree_insert_fixup 함수에서 추가적인 작업이 수행되지않고 새로운 노드의 삽입
  return new_node;
}

// 노드 삽입 후 불균형을 잡는 함수
void rbtree_insert_fixup(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle;
  int is_left = node == parent->left; // 현재 노드가 왼쪽 자식인지 여부
  int is_parent_is_left;              // 부모가 왼쪽 자식인지 여부

  //추가된 노드가 root 노드인 경우: 색만 변경
  if (node == t->root)
  {
    node->color = RBTREE_BLACK;
    return;
  }

  //부모가 BLACK인 경우: 변경 없음
  if (parent->color == RBTREE_BLACK)
    return;

  is_parent_is_left = grand_parent->left == parent;
  uncle = (is_parent_is_left) ? grand_parent->right : grand_parent->left;

  //부모와 부모의 형제가 모두 RED인 경우
  //부모와 부모의 형제 노드가 모두 빨강이고, 부모와 형제의 색을 검정으로 변경하고, 부모의 부모 노드(할아버지)의 색을 빨강으로 변경
  if (uncle->color == RBTREE_RED)
  {
    parent->color = RBTREE_BLACK;
    uncle->color = RBTREE_BLACK;
    grand_parent->color = RBTREE_RED;
    rbtree_insert_fixup(t, grand_parent);
    
    return;
  }

  if (is_parent_is_left) //부모 노드가 왼쪽 자식인 경우
  {
    if (is_left) //현재 노드가 왼쪽 자식인지
    //부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 왼쪽 자식인 경우
    {
      right_rotate(t, parent);
      exchange_color(parent, parent->right);
      return;
    }

    //부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 오른쪽 자식인 경우
    left_rotate(t, node);
    right_rotate(t, node);
    exchange_color(node, node->right);
    return;
  }

  if (is_left)
  {
    //부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 왼쪽 자식인 경우
    //그 이유는 is_parent_is_left가 false이면 skip해서 if(is_left)로 와서 조건을 참고해서 현재 노드가 왼쪽 자식인 경우 다음 코드를 실행
    right_rotate(t, node);
    left_rotate(t, node);
    exchange_color(node, node->left);
    return;
  }

  //부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 오른쪽 자식인 경우
  left_rotate(t, parent);
  exchange_color(parent, parent->left);
}

//오른쪽으로 회전하는 함수
void right_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_right = node->right;

  //부모가 루트인 경우: 현재 노드를 루트로 지정(노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  { //root 노드가 아니라면, 노드의 부모를 grand_parent로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent; //노드를 grand_parent의 자식으로 변경 (양방향 연결)
  parent->parent = node;       //parent의 부모를 노드로 변경
  node->right = parent;        //parent를 노드의 자식으로 변경 (양방향 연결)
  node_right->parent = parent; //노드의 자식의 부모를 parent로 변경
  parent->left = node_right;   //노드의 자식을 부모의 자식으로 변경 (양방향 연결)
}

//왼쪽으로 회전하는 함수
void left_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_left = node->left;

  //부모가 루트인 경우: 현재 노드를 루트로 지정(노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  { // 1-1) 노드의 부모를 grand_parent로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경 (양방향 연결)
  parent->parent = node;       // 2-1) parent의 부모를 노드로 변경
  node->left = parent;         // 2-2) parent를 노드의 자식으로 변경 (양방향 연결)
  parent->right = node_left;   // 3-1) 노드의 자식의 부모를 parent로 변경
  node_left->parent = parent;  // 3-2) 노드의 자식을 부모의 자식으로 변경 (양방향 연결)
}

//탐색 1 - key 탐색
//key에 해당하는 노드를 반환하는 함수
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root; //현재 노드를 트리의 루트로 지정한다
  while (current != t->nil) //현재 노드가 트리의 끝을 나타내는 nil 노드가 아닐 때
  {
    if (key == current->key) //현재 노드의 키 값이 주어진 키 값과 일치한다면 현재 노드를 반환
      return current;
    else
      current = (key < current->key) ? current->left : current->right;
      //주어진 키 값이 현재 노드의 키 값보다 작으면 왼쪽 자식으로 이동하고, 크면 오른쪽 자식으로 이동한다
  }
  return NULL; // 일치하는 키 값을 가진 노드를 찾지 못한 경우, NULL을 반환
}

//탐색 2 - 최소값을 가진 node 탐색
//key가 최소값에 해당하는 노드를 반환하는 함수
node_t *rbtree_min(const rbtree *t)
{
  node_t *current = t->root;
  while (current->left != t->nil)
    current = current->left;
  return current;
}

//탐색 3 - 최대값을 가진 node 탐색
//key가 최대값에 해당하는 노드를 반환하는 함수
node_t *rbtree_max(const rbtree *t)
{
  node_t *current = t->root;
  while (current->right != t->nil)
    current = current->right;
  return current;
}


//t를 inorder(중위순회)로 `n`번 순회한 결과를 `arr`에 담는 함수, 왼쪽 서브트리를 순회하고, 현재 노드를 방문, 오른쪽 서브트리 순회
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  node_t *current = rbtree_min(t); //트리의 최솟값 노드를 찾아 current에 할당한다
  arr[0] = current->key; //배열의 첫번째 원소에 현재 노드의 키 값을 저장한다
  for (int i = 1; i < n; i++) //배열의 두번째 원소부터 마지막 원소까지 반복문 실행, 첫번쨰 원소는 이미 처리했으므로
  {
    if (current == t->nil) //현재 노드가 트리의 끝을 나타내는 nil 노드일 경우 반복문을 탈출한다
      break;                       
    current = get_next_node(t, current); //다음 노드를 가져온다
    if (current == t->nil) //다음 노드가 트리의 끝을 나타태는 nil 노드일 경우 반복문 탈출
      break;               
    arr[i] = current->key; // 현재 노드의 key 값을 배열의 i번째 원소에 저장한다
  }
  return 0;
}


//노드를 삭제하는 함수
int rbtree_erase(rbtree *t, node_t *delete)
{
  node_t *remove; // 트리에서 없어질 노드
  node_t *remove_parent, *replace_node;
  int is_remove_black, is_remove_left;

  // Step 1) delete 삭제 후 대체할 replace_node 찾기
  // Step 1-1) delete 노드의 자식이 둘인 경우: delete의 키를 후계자 노드의 키값으로 대체, 노드의 색은 delete의 색 유지
  if (delete->left != t->nil && delete->right != t->nil)
  {
    remove = get_next_node(t, delete); // 후계자 노드 (오른쪽 서브트리에서 가장 작은 노드)
    replace_node = remove->right;      // 대체할 노드: 지워질 노드인 후계자는 항상 왼쪽 자식이 없기 때문에, 자식이 있다면 오른쪽 자식 하나뿐임
    delete->key = remove->key;         // delete의 키를 후계자 노드의 키값으로 대체 (색은 변경 X)
  }
  else
  { // Step 1-2) delete 노드의 자식이 없거나 하나인 경우: delete 노드를 자식으로 대체, 노드의 색도 대체되는 노드의 색으로 변경
    remove = delete;
    // 대체할 노드: 자식이 있으면 자식노드로, 없으면 nil 노드로 대체
    replace_node = (remove->right != t->nil) ? remove->right : remove->left;
  }
  remove_parent = remove->parent;

  // Step 2) remove 노드 제거하기
  //remove 노드가 루트인 경우
  if (remove == t->root)
  {
    t->root = replace_node;        // 대체할 노드를 트리의 루트로 지정
    t->root->color = RBTREE_BLACK; // 루트 노드는 항상 BLACK
    free(remove);
    return 0; // 불균형 복구 함수 호출 불필요 (제거 전 트리에 노드가 하나 혹은 두개이므로 불균형이 발생하지 않음)
  }

  // Step 2-1) 'remove의 부모'와 'remove의 자식' 이어주기
  is_remove_black = remove->color; // remove 노드 제거 전에 지워진 노드의 색 저장
  is_remove_left = remove_parent->left == remove;

  // Step 2-1-1) 자식 연결
  if (is_remove_left) // remove가 왼쪽 자식이었을 경우: remove 부모의 왼쪽에 이어주기
    remove_parent->left = replace_node;
  else // remove가 오른쪽 자식이었을 경우: remove 부모의 오른쪽에 이어주기
    remove_parent->right = replace_node;

  // Step 2-1-2) 부모도 연결 (양방향 연결)
  replace_node->parent = remove_parent;
  free(remove);

   
  // Step 3)불균형 복구 함수 호출
  if (is_remove_black) //remove 노드가 검정색일 경우 
    rbtree_erase_fixup(t, remove_parent, is_remove_left);
  return 0;
}

// 노드를 삭제하고 불균형을 잡아준다
// `parent`: extra_black이 부여된 노드의 부모
// `is_left`: extra_black이 부여된 노드가 왼쪽 자식인지 여부
void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left)
{
  // 삭제 후 대체한 노드가 RED (Red & Black): BLACK으로 변경
  node_t *extra_black = is_left ? parent->left : parent->right; //is_left가 참이면..
  if (extra_black->color == RBTREE_RED)
  {
    extra_black->color = RBTREE_BLACK;
    return;
  }

  node_t *sibling = is_left ? parent->right : parent->left;
  node_t *sibling_left = sibling->left;
  node_t *sibling_right = sibling->right;

  if (sibling->color == RBTREE_RED)
  { //형제가 RED
    if (is_left)
      left_rotate(t, sibling);
    else
      right_rotate(t, sibling);
    exchange_color(sibling, parent);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  node_t *near = is_left ? sibling_left : sibling_right;    // 형제의 자식 중 extra_black으로부터 가까운 노드
  node_t *distant = is_left ? sibling_right : sibling_left; // 형제의 자식 중 extra_black으로부터 먼 노드

  if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  { //형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
    right_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  if (is_left && distant->color == RBTREE_RED)
  { //형제가 BLACK, 형제의 더 먼 자식이 RED
    left_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  { //형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
    left_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  if (distant->color == RBTREE_RED)
  { //형제가 BLACK, 형제의 더 먼 자식이 RED
    right_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  //형제가 BLACK, 형제의 자식이 둘 다 BLACK
  sibling->color = RBTREE_RED;

  if (parent != t->root)
    rbtree_erase_fixup(t, parent->parent, parent->parent->left == parent);
}

void exchange_color(node_t *a, node_t *b)
{
  int tmp = a->color; //tmp를 선언하고, 노드 'a'의 색상을 임시로 저장한다
  a->color = b->color; //노드 a의 색상을 b의 색상으로 바꾼다
  b->color = (tmp == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED; //삼항연산자로 b의 색상을 정한다
  //tmp==RBTREE_BLACK이 참이면 b->color에 1이 대입되고, 결과적으로 참이 된다
}

//키 값을 기준으로 다음 노드를 반환하는 함수
node_t *get_next_node(const rbtree *t, node_t *p)
{
  node_t *current = p->right; //주어진 노드 p의 오른쪽 자식을 current로 지정
  if (current == t->nil) 
  //현재 노드의 오른쪽 자식이 트리의 끝을 나타내는 nil 노드인 경우, 즉 오른쪽 자식이 없는 경우
  {
    current = p; //주어진 노드를 p로 지정
    while (1) //무한 루프를 시작한다
    {
      if (current->parent->right == current) // current가 오른쪽 자식인 경우라면 
        current = current->parent;           // 부모 노드로 이동 후 이어서 탐색
      else
        return current->parent; // current가 왼쪽 자식인 경우 부모 리턴
    }
  }
  while (current->left != t->nil) // 왼쪽 자식이 있는 경우
    current = current->left;      // 가장 왼쪽 끝으로 이동
  return current; //현재 노드를 반환한다
}
