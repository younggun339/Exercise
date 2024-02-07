#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

// 재귀호출을 위해 미리 선언
void resolve_double_black_left(rbtree *, node_t *);
void resolve_double_black_right(rbtree *, node_t *);

/*--------------------------AUX FUNC--------------------------*/

node_t *create_node(rbtree *t, const key_t key) {
  // 새로운 노드 할당 및 반환
  node_t *new_p = (node_t *)calloc(1, sizeof(node_t));
  new_p->color = RBTREE_RED; // 빨강으로 초기화
  new_p->key = key;
  new_p->parent = t->nil;
  new_p->left = t->nil;
  new_p->right = t->nil;

  return new_p;
}

void delete_node(rbtree *t, node_t *p) {
  // p와 그 자식들을 재귀적으로 삭제
  if (p != t->nil) {
    delete_node(t, p->left);
    delete_node(t, p->right);
    free(p);
  }
}

void transplant(rbtree *t, node_t *x_p, node_t *y_p) {
  // x를 루트로 하는 서브트리를 y를 루트로 하는 서브트리 자리에 이식
  if (y_p->parent->left == y_p) { // y가 왼쪽 자식인지 확인
    y_p->parent->left = x_p;
  } else {
    y_p->parent->right = x_p;
  }
  x_p->parent = y_p->parent;

  if (y_p == t->root) { // 루트 업데이트
    t->root = x_p;
  }
}

node_t *find_successor(rbtree *t, node_t *p) {
  // 직후원소 검색 및 반환
  node_t *search_p = p->right;

  if (search_p == t->nil) {
    // 직후원소 없음
    printf("find_successor(): no succesor to be found\n");
    return NULL;
  }

  while (search_p->left != t->nil) {
    search_p = search_p->left;
  }

  return search_p;
}

void rotate_cw(rbtree *t, node_t *p) {
  // p와 그 왼쪽 자식을 시계방향으로 회전
  if (p->left == t->nil) {
    printf("rotate_cw(): 왼쪽 자식이 없어서 회전 불가\n");
    return;
  }
  // 임시 포인터 배정
  node_t *parent_p = p->parent;
  node_t *left_p = p->left;
  node_t *lr_p = p->left->right;

  //parent => left 연결
  if (parent_p->left == p) { // p가 p의 부모의 왼쪽 자식인지 여부 확인
    parent_p->left = left_p;
  } else {
    parent_p->right = left_p;
  }

  p->parent = left_p; // p => left 연결
  p->left = lr_p; // p => left.right 연결

  left_p->parent = parent_p; // left => parent 연결
  left_p->right = p; // left => p 연결

  lr_p->parent = p; // left.right => p 연결
  
  if (t->root == p) {
    t->root = left_p; // p가 루트였다면 루트를 변경
  }
}

void rotate_ccw(rbtree *t, node_t *p) {
  // p와 그 오른쪽 자식을 반시계방향으로 회전
  if (p->right == t->nil) {
    printf("rotate_ccw(): 오른쪽 자식이 없어서 회전 불가\n");
    return;
  }
  // 임시 포인터 배정
  node_t *parent_p = p->parent;
  node_t *right_p = p->right;
  node_t *rl_p = p->right->left;

  //parent => right 연결
  if (parent_p->left == p) { // p가 p의 부모의 왼쪽 자식인지 여부 확인
    parent_p->left = right_p;
  } else {
    parent_p->right = right_p;
  }

  p->parent = right_p; // p => right 연결
  p->right = rl_p; // p => right.left 연결

  right_p->parent = parent_p; // right => parent 연결
  right_p->left = p; // right => p 연결

  rl_p->parent = p; // right.left => p 연결
  
  if (t->root == p) {
    t->root = right_p; // p가 루트였다면 루트를 변경
  }
}

void resolve_double_red(rbtree *t, node_t *p) {
  // 이중 빨강을 재귀적으로 해결
  // p와 부모가 모두 빨강이라고 가정
  if (p->color != RBTREE_RED || p->parent->color != RBTREE_RED) {
    // 이중 빨강이 아님
    return;
  }

  node_t *uncle_p; // p의 삼촌
  // p의 부모가 p의 할아버지의 왼쪽 자식인지 여부 확인
  char parent_is_left = p->parent->parent->left == p->parent;
  if (parent_is_left) {
    // 삼촌은 오른쪽 자식
    uncle_p = p->parent->parent->right;
  } else {
    // 삼촌은 왼쪽 자식
    uncle_p = p->parent->parent->left;
  }

  if (uncle_p->color == RBTREE_RED) {
    // 경우 1: 삼촌이 빨강
    p->parent->color = RBTREE_BLACK;
    uncle_p->color = RBTREE_BLACK;
    p->parent->parent->color = RBTREE_RED;
    // p의 할아버지가 빨강이 된 후 재귀호출
    resolve_double_red(t, p->parent->parent);
  } else {
    // 경우 2: 삼촌이 검정
    // p가 p의 부모의 왼쪽 자식인지 여부 확인
    char p_is_left = p->parent->left == p;
    if (parent_is_left) {
      if (!p_is_left) {
        rotate_ccw(t, p->parent); // 방향정렬
        p = p->left;
      }
      // 검빨 드리프트
      p->parent->parent->color = RBTREE_RED;
      p->parent->color = RBTREE_BLACK;
      rotate_cw(t, p->parent->parent);
    } else {
      // 좌우 반전
      if (p_is_left) {
        rotate_cw(t, p->parent); // 방향정렬
        p = p->right;
      }
      // 검빨 드리프트
      p->parent->parent->color = RBTREE_RED;
      p->parent->color = RBTREE_BLACK;
      rotate_ccw(t, p->parent->parent);
    }
  }
}

void resolve_double_black_left(rbtree *t, node_t *parent_p) {
  // 이중 검정 해결
  // parent_p의 왼쪽 자식 위치에 청구서가 있음
  if (parent_p == t->nil) {
    // 이중 검정이 루트에 해당함: 루트를 검정으로 바꾸면 해결
    t->root->color = RBTREE_BLACK;
  } else if (parent_p->left->color == RBTREE_RED) {
    // 빨강을 검정으로 바꾸면 해결 (이중 검정이 아님)
    parent_p->left->color = RBTREE_BLACK;
  } else {
    // 이중 검정 해결
    node_t *sibling_p = parent_p->right;

    if (sibling_p->color == RBTREE_RED) {
      // 경우 1: 형제가 빨강
      // 검빨 드리프트
      rotate_ccw(t, parent_p);
      parent_p->color = RBTREE_RED;
      sibling_p->color = RBTREE_BLACK;
      // 형제를 다시 찾은 후 다른 경우로 이동 (형제는 무조건 검정)
      sibling_p = parent_p->right;
    }

    // (형제가 검정인 상태로 코드 진입)
    if (sibling_p->left->color == RBTREE_BLACK && sibling_p->right->color == RBTREE_BLACK) {
      // 경우 2: 형제의 자식이 모두 검정
      sibling_p->color = RBTREE_RED;
      // 재귀 호출
      if (parent_p->parent->left == parent_p) { // parent is left
        resolve_double_black_left(t, parent_p->parent);
      } else {
        resolve_double_black_right(t, parent_p->parent); 
      }
    } else {
      if (sibling_p->right->color == RBTREE_BLACK) {
        // 경우 3: 형제의 오른쪽 자식이 검정
        // 검빨 드리프트 후 경우 4로 이동
        rotate_cw(t, sibling_p);
        sibling_p->color = RBTREE_RED;
        sibling_p->parent->color = RBTREE_BLACK;
        sibling_p = parent_p->right; // sibling_p->parent와 같음
      }
      // (오른쪽 자식이 빨강인 상태로 코드 진입)
      // 경우 4: 형제의 오른쪽 자식이 빨강
      color_t temp_color;
      rotate_ccw(t, parent_p);
      temp_color = parent_p->color;
      parent_p->color = sibling_p->color;
      sibling_p->color = temp_color;
      sibling_p->right->color = RBTREE_BLACK;
    }
  }
}

void resolve_double_black_right(rbtree *t, node_t *parent_p) {
  // 이중 검정 해결
  // parent_p의 오른쪽 자식 위치에 청구서가 있음
  if (parent_p == t->nil) {
    // 이중 검정이 루트에 해당함: 루트를 검정으로 바꾸면 해결
    t->root->color = RBTREE_BLACK;
  } else if (parent_p->right->color == RBTREE_RED) {
    // 빨강을 검정으로 바꾸면 해결 (이중 검정이 아님)
    parent_p->right->color = RBTREE_BLACK;
  } else {
    // 이중 검정 해결
    node_t *sibling_p = parent_p->left;

    if (sibling_p->color == RBTREE_RED) {
      // 경우 1: 형제가 빨강
      // 검빨 드리프트
      rotate_cw(t, parent_p);
      parent_p->color = RBTREE_RED;
      sibling_p->color = RBTREE_BLACK;
      // 형제를 다시 찾은 후 다른 경우로 이동 (형제는 무조건 검정)
      sibling_p = parent_p->left;
    }

    // (형제가 무조건 검정인 상태로 코드 진입)
    if (sibling_p->left->color == RBTREE_BLACK && sibling_p->right->color == RBTREE_BLACK) {
      // 경우 2: 형제의 자식이 모두 검정
      sibling_p->color = RBTREE_RED;
      // 재귀 호출
      if (parent_p->parent->left == parent_p) { // parent is left
        resolve_double_black_left(t, parent_p->parent);
      } else {
        resolve_double_black_right(t, parent_p->parent); 
      }
    } else {
      if (sibling_p->left->color == RBTREE_BLACK) {
        // 경우 3: 형제의 왼쪽 자식이 검정
        // 검빨 드리프트 후 경우 4로 이동
        rotate_ccw(t, sibling_p);
        sibling_p->color = RBTREE_RED;
        sibling_p->parent->color = RBTREE_BLACK;
        sibling_p = sibling_p->parent;
      }
      // (왼쪽 자식이 빨강인 상태로 코드 진입)
      // 경우 4: 형제의 왼쪽 자식이 빨강
      color_t temp_color;
      rotate_cw(t, parent_p);
      temp_color = parent_p->color;
      parent_p->color = sibling_p->color;
      sibling_p->color = temp_color;
      sibling_p->left->color = RBTREE_BLACK;
    }
  }
}

void store_key(const rbtree *t, key_t *arr, const size_t n, node_t *cur_node, int *idx) {
  // 중위 순회로 cur_node의 key를 arr[idx]에 저장
  if (*idx < n && cur_node != t->nil) {
    store_key(t, arr, n, cur_node->left, idx);
    arr[*idx] = cur_node->key;
    *idx += 1;
    store_key(t, arr, n, cur_node->right, idx);
  }
}

/*-------------------------DEBUG FUNC-------------------------*/

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
  // node_arr[i][j]: 깊이가 i인 (j+1)번째 노드 **2차원 배열 접근이 불가능해 1차원 배열로 구현
    node_t **node_arr = calloc( (render_depth +1) * pow_2(render_depth), sizeof(node_t *));

  // node_idx_list[i]: node_arr[i]의 현재 인덱스
  int *node_idx_list = (int *) calloc(render_depth +1, sizeof(int));

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
        // nil은 .으로 출력
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
  printf("\x1b[0m" "\n"); // 색 리셋

  free(node_arr);
  free(node_idx_list);
}

/*-------------------------TODO FUNC-------------------------*/

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;

  p->nil = nil;
  p->root = nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *new_p = create_node(t, key);

  if (t->root == t->nil) {
    // 빈 트리의 루트로 삽입
    t->root = new_p;
  } else {
    // 빈 트리가 아님
    // 1. 위치 확인 및 삽입
    node_t *search_p = t->root; // 검색 위치
    while (1) {
      // 삽입 위치를 검색
      if (new_p->key < search_p->key) {
        if (search_p->left == t->nil) {
          // 즉시 삽입
          search_p->left = new_p;
          new_p->parent = search_p;
          break;
        } else {
        search_p = search_p->left; // 왼쪽으로 이동
        }
      } else {
        if (search_p->right == t->nil) {
          // 즉시 삽입
          search_p->right = new_p;
          new_p->parent = search_p;
          break;
        } else {
          search_p = search_p->right; // 오른쪽으로 이동
        }
      }
    }

    // 2. 이중 빨강 해결
    resolve_double_red(t, new_p);
  }

  // 3. 루트를 검정으로
  t->root->color = RBTREE_BLACK;

  return new_p;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *search_p = t->root; // 검색 위치
  while (search_p != t->nil) {
    if (key == search_p->key) {
      // 키 발견
      return search_p;
    }

    if (key < search_p->key) {
      search_p = search_p->left; // 왼쪽으로 이동
    } else {
      search_p = search_p->right; // 오른쪽으로 이동
    }
  }
  // 탐색 실패
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement min
  if (t->root == t->nil) {
    return NULL;
  }

  node_t *search_p = t->root; // 검색 위치
  while (search_p->left != t->nil) {
    search_p = search_p->left;
  }
  return search_p;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement max
  if (t->root == t->nil) {
    return NULL;
  }

  node_t *search_p = t->root; // 검색 위치
  while (search_p->right != t->nil) {
    search_p = search_p->right;
  }
  return search_p;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  if (p == NULL || p == t->nil) {
    printf("rbtree_erase(): received nil!\n");
    return 0;
  }

  node_t *resolve_p; // 삭제된 위치에 도착한 노드의 부모
  
  char p_is_left = 0;
  p_is_left = p->parent->left == p;

  if (p->left == t->nil) {
    // right를 이식 (right만 존재하거나 자식 없음)
    resolve_p = p->parent;

    transplant(t, p->right, p);
    if (p->color == RBTREE_BLACK) {
      if (p_is_left) {
        resolve_double_black_left(t, resolve_p);
      } else {
        resolve_double_black_right(t, resolve_p);
      }
    }
  } else if (p->right == t->nil) {
    // left를 이식 (left만 존재)
    resolve_p = p->parent;

    transplant(t, p->left, p);
    if (p->color == RBTREE_BLACK) {
      if (p_is_left) {
        resolve_double_black_left(t, resolve_p);
      } else {
        resolve_double_black_right(t, resolve_p);
      }
    }
  } else {
    // 둘 다 존재
    node_t *scsr_p = find_successor(t, p);
    color_t scsr_color = scsr_p->color;
    if (scsr_p->parent == p) {
      // 직후원소가 오른쪽 자식인 경우
      resolve_p = scsr_p;
      if (p_is_left) { // parent => scsr 연결
        p->parent->left = scsr_p;
      } else {
        p->parent->right = scsr_p;
      }
      p->left->parent = scsr_p; // left => scsr 연결
      scsr_p->parent = p->parent; // scsr => parent 연결
      scsr_p->left = p->left; // scsr => left 연결

      scsr_p->color = p->color;

      if (p == t->root) {
        t->root = scsr_p;
      }
      if (scsr_color == RBTREE_BLACK) {
        resolve_double_black_right(t, resolve_p);
      }
    } else {
      // 직후원소와 자리 교체
      resolve_p = scsr_p->parent;
      transplant(t, scsr_p->right, scsr_p);

      if (p_is_left) { // parent => scsr 연결
        p->parent->left = scsr_p;
      } else {
        p->parent->right = scsr_p;
      }
      p->left->parent = scsr_p; // left => scsr 연결
      p->right->parent = scsr_p;// right => scsr 연결
      scsr_p->parent = p->parent; // scsr => parent 연결
      scsr_p->left = p->left; // scsr => left 연결
      scsr_p->right = p->right; // scsr => right 연결

      scsr_p->color = p->color;

      if (p == t->root) {
        t->root = scsr_p;
      }
      if (scsr_color == RBTREE_BLACK) {
        resolve_double_black_left(t, resolve_p);
      }
    }
  }
  
  free(p);
  return 1;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int *idx = calloc(1, sizeof(int));
  store_key(t, arr, n, t->root, idx);
  free(idx);
  return 1;
}