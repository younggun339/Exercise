#include "rbtree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

node_t *get_grandparent_ptr(rbtree *t, node_t *n) {
	if ((n != t->nil) && (n->parent != t->nil))
		return n->parent->parent;
	else
		return t->nil;
}

node_t *get_uncle_ptr(rbtree *t, node_t *n) {
	node_t *g = get_grandparent_ptr(t, n);
	if (g == t->nil)
		return t->nil;
	if (n->parent == g->left)
		return g->right;
	else
		return g->left;
}

void rotate_left(rbtree *t, node_t *n) {
	node_t *c = n->right;
	node_t *p = n->parent;

	if (c->left != t->nil)
		c->left->parent = n;
	n->right = c->left;
	n->parent = c;
	c->left = n;
	c->parent = p;
	if (p != t->nil) {
		if (p->left == n)
			p->left = c;
		else
			p->right = c;
	} else {
		t->root = c;
	}
}

void rotate_right(rbtree *t, node_t *n) {
	node_t *c = n->left;
	node_t *p = n->parent;

	n->left = c->right;
	if (c->right != t->nil)
		c->right->parent = n;
	n->parent = c;
	c->right = n;
	c->parent = p;
	if (p != t->nil) {
		if (p->right == n)
			p->right = c;
		else
			p->left = c;
	} else {
		t->root = c;
	}
}

node_t *sibling(node_t *n) {
	if (n == n->parent->left)
		return n->parent->right;
	else
		return n->parent->left;
}


void replace_node(node_t *n, node_t * child) {
	/*
	*앞에서 n의 부모가 NULL이 되는 경우를 delete_case에 오지 않게 미리 처리해주면 된다.
	*/
	child->parent = n->parent;
	if (n->parent->left == n)
		n->parent->left = child;
	else if (n->parent->right == n)
		n->parent->right = child;
}




static int pow_2(int x) {
  // 2**x 계산 함수
  return 1 << x;
}

static void record_keys(rbtree *t, node_t *p, node_t **node_arr, int *node_idx_list, int cur_depth, int render_depth) {
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

void	  rbtree_traversal(rbtree *t, node_t *root, key_t *arr, int *idx) {
	if (root != t->nil) {
		rbtree_traversal(t, root->left, arr, idx);
		arr[(*idx)++] = root->key;
		rbtree_traversal(t, root->right, arr, idx);
	}
}

void visualize_tree(rbtree *t, int render_depth) {
  // 트리 시각화 함수
  // depth는 루트를 0으로 계산
  // key_arr[i][j]: 깊이가 i인 (j+1)번째 원소의 키 **2차원 배열 접근이 불가능해 1차원 배열로 구현
    node_t **node_arr = calloc( (render_depth +1) * pow_2(render_depth), sizeof(node_t *));

  // key_idx_list[i]: key_arr[i]의 현재 인덱스
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



