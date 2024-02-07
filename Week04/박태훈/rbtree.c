#include "rbtree.h"
#include "rbtreeUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SPACE 3

node_t* new_node(void) {
	node_t* node = (node_t*)calloc(1, sizeof(node_t));

	if (node == NULL) {
		printf("메모리 할당 거부됨.\n");
		return NULL;
	}

	return node;
}

// rb트리 내의 모든 노드 삭제(rbtree 포인터, node_t 포인터)
// ->
void clear_tree(rbtree* t, node_t* node) {
	if (t == NULL || node == t->nil)
		return;
	if (node == NULL)
		node = t->root;

	clear_tree(t, node->left);
	clear_tree(t, node->right);
	free(node);
}

node_t* node_insert(rbtree* t, node_t** node, node_t* parent, const key_t key) {
	if ((*node) == NULL) {
		printf("노드 정의 올바르지 않음!\n");
		return NULL;
	}

	if ((*node) == t->nil) {
		(*node) = new_node();
		(*node)->key = key;
		(*node)->left = t->nil;
		(*node)->right = t->nil;
		(*node)->parent = parent;
		(*node)->color = RBTREE_RED;
		return *node;
	}

	if (key <= (*node)->key)
		return node_insert(t, &((*node)->left), *node, key);
	else
		return node_insert(t, &((*node)->right), *node, key);
}

node_t* node_find(const rbtree* t, node_t* node, const key_t key, const int depth) {
	if (t == NULL || node == NULL) {
		printf("트리나 노드 정의 잘못됨!\n");
		return NULL;
	}

	if (node == t->nil) {
		printf("값을 찾지 못함!\n");
		return NULL;
	}

	if (node->key == key) {
		printf("%d:%d - %d번째 순회 찾음.\n", key, node->key, depth);
		return node;
	}
	else if (key < node->key)
		return node_find(t, node->left, key, depth + 1);
	else
		return node_find(t, node->right, key, depth + 1);
}

node_t* node_min(const rbtree* t, node_t* node) {
	if (node == NULL) {
		printf("트리에 아무 노드도 없음!\n");
		return NULL;
	}

	if (node->left == t->nil || node == t->nil)
		return node;
	else
		return node_min(t, node->left);
}

node_t* node_max(const rbtree* t, node_t* node) {
	if (node == NULL) {
		printf("트리에 아무 노드도 없음!\n");
		return NULL;
	}

	if (node->right == t->nil || node == t->nil)
		return node;
	else
		return node_max(t, node->right);
}

// rb트리 내의 노드 반시계방향 회전(rbtree 포인터, 회전할 노드의 포인터)
// -> 성공 여부(0, -1) 반환
int rotateR(rbtree* t, node_t* p) {
	if (t == NULL)
		t = new_rbtree();

	if (p == NULL) {
		printf("회전시킬 노드 정의 잘못됨!\n");
		return -1;
	}

	if (p == NULL || p->left == t->nil) {
		printf("회전시킬 노드 정의 잘못됨!\n");
		return -1;
	}

	// 부모자식 역전될 노드 설정
	node_t* upsideDown_node = p->left;

	// 꺾인 손자를 자신의 왼쪽 아들로 1번 이산->
	p->left = upsideDown_node->right;
	// 꺾인 손자의 부모를 자신으로 1번 경합<-
	if (upsideDown_node->right != t->nil)
		upsideDown_node->right->parent = p;

	// 자신의 부모를 역전될 노드로 2번 이산->
	upsideDown_node->parent = p->parent;
	// 자신이 루트라면
	if (p == t->root) {
		upsideDown_node->parent = t->nil;
		t->root = upsideDown_node;
	}
	// 타겟 노드가 오른쪽으로 뻗어나간 애라면 2번 경합<-
	else if (p->parent->right == p)
		p->parent->right = upsideDown_node;
	// 타겟 노드가 왼쪽으로 뻗어나간 애라면 2번 경합<-
	else
		p->parent->left = upsideDown_node;

	// 자신을 역전될 노드의 오른쪽 자식으로 3번 이산->
	upsideDown_node->right = p;
	// 역전될 노드를 자신의 부모로 3번 경합<-
	p->parent = upsideDown_node;

	return 0;
}

// rb트리 내의 노드 반시계방향 회전(rbtree 포인터, 회전할 노드의 포인터)
// -> 성공 여부(0, -1) 반환
int rotateL(rbtree* t, node_t* p) {
	if (t == NULL)
		t = new_rbtree();

	if (p == NULL) {
		printf("회전시킬 노드 정의 잘못됨!\n");
		return -1;
	}

	if (p == NULL || p->right == t->nil) {
		printf("회전시킬 노드 정의 잘못됨!\n");
		return -1;
	}

	// 부모자식 역전될 노드 설정
	node_t* upsideDown_node = p->right;

	// 꺾인 손자를 오른쪽 아들로 1번 이산->
	p->right = upsideDown_node->left;
	// 꺾인 손자의 부모를 자신으로 1번 경합<-
	if (upsideDown_node->left != t->nil)
		upsideDown_node->left->parent = p;

	// 자신의 부모를 역전될 노드로 2번 이산->
	upsideDown_node->parent = p->parent;
	// 자신이 루트라면
	if (p == t->root) {
		t->root = upsideDown_node;
	}
	// 타겟 노드가 오른쪽으로 뻗어나간 애라면 2번 경합<-
	else if (p->parent->right == p)
		p->parent->right = upsideDown_node;
	// 타겟 노드가 왼쪽으로 뻗어나간 애라면 2번 경합<-
	else
		p->parent->left = upsideDown_node;

	// 역전될 노드의 왼쪽 자식을 자신으로 3번 이산->
	upsideDown_node->left = p;
	// 자신의 부모를 역전될 노드로 3번 경합<-
	p->parent = upsideDown_node;

	return 0;
}

void rbtree_transplant(rbtree* t, node_t* u, node_t* v) {
	if (u->parent == t->nil)
		t->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}


int make_inorder_array(const rbtree* t, key_t* arr, node_t* node, int i) {
	if (node == NULL) {
		printf("잘못된 노드 연결 감지!\n");
		return 0;
	}

	if (node == t->nil)
		return i;

	i = make_inorder_array(t, arr, node->left, i);
	arr[i++] = node->key;
	i = make_inorder_array(t, arr, node->right, i);
	return i;
}


//// 검사 함수
bool isDoubleRed(rbtree* t, node_t* ins_node)
{
	if (t == NULL || t->root == NULL || ins_node == NULL || ins_node->color == RBTREE_BLACK)
		return false;

	if (ins_node->parent == t->root)
		return false;

	return ins_node->parent->color == RBTREE_RED;
}


//// 검사 및 수정 함수
void rootBlack(rbtree* t)
{
	if (t == NULL || t->root == NULL)
		return;

	if (t->root->color == RBTREE_RED)
		t->root->color = RBTREE_BLACK;
}

void rb_insert_fixup(rbtree* t, node_t* new_node)
{
	if (t == NULL || new_node == NULL || new_node == t->nil)
		return;

	// 리턴 조건: 루트까지 올라왔다면
	if (new_node == t->root)
		return;

	// 룰4 위반 검사
	if (isDoubleRed(t, new_node)) {
		// 조건 1-1. 부모가 조부모로부터 왼쪽으로 뻗어나왔으면 (동시에 부모가 루트가 아니면)
		if (new_node->parent->parent->left == new_node->parent) {
			// unc_node: 삼촌 노드 정의
			node_t* unc_node = new_node->parent->parent->right;
			// 조건 1-2. 삼촌 노드가 빨강이면
			if (unc_node->color == RBTREE_RED) {
				new_node->parent->color = RBTREE_BLACK;
				unc_node->color = RBTREE_BLACK;
				new_node->parent->parent->color = RBTREE_RED;
			}
			else {
				// 조건 1-3. 자신이 부모로부터 오른쪽으로 뻗어나왔으면, 즉 조부모로부터 꺾인 자식이면
				if (new_node->parent->right == new_node) {
					// 왼쪽 회전해서 편 자식으로 만들고, 실패 시 바로 반환
					if (rotateL(t, new_node->parent) != 0)
						return;
				}
				// 자신이 부모로부터 오른쪽으로 뻗어나왔으면, 검빨드리프트를 위해 자신을 부모 노드로 설정
				else if (new_node->parent->left == new_node)
					new_node = new_node->parent;
				// 편 자식으로 되면, 오른쪽 검빨드리프트, 실패 시 바로 반환
				new_node->color = RBTREE_BLACK;
				new_node->parent->color = RBTREE_RED;
				if (rotateR(t, new_node->parent) != 0)
					return;
			}
		}
		// 조건 2-1. 부모가 조부모로부터 오른쪽으로 뻗어나왔으면 (동시에 부모가 루트가 아니면)
		else if (new_node->parent->parent->right == new_node->parent) {
			// unc_node: 삼촌 노드 정의
			node_t* unc_node = new_node->parent->parent->left;
			// 조건 2-2. 삼촌 노드가 빨강이면
			if (unc_node->color == RBTREE_RED) {
				new_node->parent->color = RBTREE_BLACK;
				unc_node->color = RBTREE_BLACK;
				new_node->parent->parent->color = RBTREE_RED;
			}
			else {
				// 조건 2-3. 자신이 부모로부터 왼쪽으로 뻗어나왔으면, 즉 조부모로부터 꺾인 자식이면
				if (new_node->parent->left == new_node) {
					// 오른쪽 회전해서 편 자식으로 만들고, 실패 시 바로 반환
					if (rotateR(t, new_node->parent) != 0)
						return;
				}
				// 자신이 부모로부터 오른쪽으로 뻗어나왔으면, 검빨드리프트를 위해 자신을 부모 노드로 설정
				else if (new_node->parent->right == new_node)
					new_node = new_node->parent;
				// 편 자식으로 되면, 왼쪽 검빨트리프트, 실패 시 바로 반환
				new_node->color = RBTREE_BLACK;
				new_node->parent->color = RBTREE_RED;
				if (rotateL(t, new_node->parent) != 0)
					return;
			}
		}
	}
	// 수정 후 루트까지 재귀 검사
	rb_insert_fixup(t, new_node->parent);
}

void rb_delete_fixup(rbtree* t, node_t* del_node)
{
	if (t == NULL || del_node == NULL)
		return;

	// 리턴 조건: 루트나 빨갱이까지 올라왔다면
	if (del_node == t->root || del_node->color == RBTREE_RED) {
		del_node->color = RBTREE_BLACK;
		return;
	}

	node_t* sib_node = NULL;
	// 조건 1. 청구서가 부모로부터 왼쪽으로 뻗어나왔으면
	if (del_node->parent->left == del_node) {
		// bro_node: 형제 노드 정의
		sib_node = del_node->parent->right;
		// 조건 1-1. 형제 노드가 빨강이면
		if (sib_node->color == RBTREE_RED) {
			// 검정으로 만들기 위해 좌회전 조진다
			sib_node->color = RBTREE_BLACK;
			del_node->parent->color = RBTREE_RED;
			rotateL(t, del_node->parent);
			// 형제 노드 갱신
			sib_node = del_node->parent->right;
		}
		// 조건 1-2. 형제의 두 자식이 모두 검정이면
		if (sib_node->left->color == RBTREE_BLACK && sib_node->right->color == RBTREE_BLACK) {
			// 형제 빨강으로 만들고 청구서 올린다
			sib_node->color = RBTREE_RED;
			// 올리면 루트까지 재귀
			rb_delete_fixup(t, del_node->parent);
		}
		// 조건 1-3. 형제의 오른쪽 자식이 빨강이 아니면
		else {
			if (sib_node->right->color != RBTREE_RED) {
				// 빨강으로 만들기 위해 우회전 조진다 (청구서 해결을 위해 일을 시켜야 하므로)
				sib_node->left->color = RBTREE_BLACK;
				sib_node->color = RBTREE_RED;
				rotateR(t, sib_node);
				// 형제 노드 갱신
				sib_node = del_node->parent->right;
			}
			// 이제 형제의 오른쪽 자식은 무조건 빨강, 형제는 무조건 검은색
			// 형제와 부모의 색깔 스왑
			sib_node->color = del_node->parent->color;
			del_node->parent->color = RBTREE_BLACK;
			// 형제의 오른쪽 자식을 검정으로, 청구서 해결
			sib_node->right->color = RBTREE_BLACK;
			// 출장 메커니즘
			rotateL(t, del_node->parent);
		}
	}
	// 조건 2. 청구서가 부모로부터 오른쪽으로 뻗어나왔으면
	else if (del_node->parent->right == del_node) {
		// bro_node: 형제 노드 정의
		sib_node = del_node->parent->left;
		// 조건 2-1. 형제 노드가 빨강이면
		if (sib_node->color == RBTREE_RED) {
			// 검정으로 만들기 위해 우회전 조진다
			sib_node->color = RBTREE_BLACK;
			del_node->parent->color = RBTREE_RED;
			rotateR(t, del_node->parent);
			// 형제 노드 갱신
			sib_node = del_node->parent->left;
		}
		// 조건 2-2. 형제의 두 자식이 모두 검정이면
		if (sib_node->left->color == RBTREE_BLACK && sib_node->right->color == RBTREE_BLACK) {
			// 형제 빨강으로 만들고 청구서 올린다
			sib_node->color = RBTREE_RED;
			// 올리면 루트까지 재귀
			rb_delete_fixup(t, del_node->parent);
		}
		// 조건 2-3. 형제의 왼쪽 자식이 빨강이 아니면
		else {
			if (sib_node->left->color != RBTREE_RED) {
				// 빨강으로 만들기 위해 우회전 조진다 (청구서 해결을 위해 일을 시켜야 하므로)
				sib_node->right->color = RBTREE_BLACK;
				sib_node->color = RBTREE_RED;
				rotateL(t, sib_node);
				// 형제 노드 갱신
				sib_node = del_node->parent->left;
			}
			// 이제 형제의 왼쪽 자식은 무조건 빨강, 형제는 무조건 검은색
			// 형제와 부모의 색깔 스왑
			sib_node->color = del_node->parent->color;
			del_node->parent->color = RBTREE_BLACK;
			// 형제의 왼쪽 자식을 검정으로, 청구서 해결
			sib_node->left->color = RBTREE_BLACK;
			// 출장 메커니즘
			rotateR(t, del_node->parent);
		}
	}
}



//// 프린트 함수
void print2DUtil(rbtree* t, node_t* node, int space)
{
	if (node == NULL)
		return;

	space += SPACE;

	print2DUtil(t, node->right, space);

	printf("\n");
	for (int i = SPACE; i < space; i++)
		printf(" ");
	if (node == t->nil)
		printf(".");
	else
		if (node->color == RBTREE_RED) {
			printf("\033[0; 31m");
			printf("%d\n", node->key);
			printf("\033[0m");
		}
		else
			printf("%d\n", node->key);

	print2DUtil(t, node->left, space);
}

void print2D(rbtree* t)
{
	print2DUtil(t, t->root, 0);
}

int pow_2(int x) {
	// 2**x 계산 함수
	return 1 << x;
}

void record_keys(rbtree* t, node_t* p, node_t** node_arr, int* node_idx_list, int cur_depth, int render_depth) {
	// visualize_tree에서 사용하기위한 배열을 재귀적으로 기록
	if (cur_depth > render_depth) {
		return;
	}

	node_arr[cur_depth * pow_2(render_depth) + node_idx_list[cur_depth]] = p;
	node_idx_list[cur_depth] += 1;
	if (p == t->nil) {
		record_keys(t, t->nil, node_arr, node_idx_list, cur_depth + 1, render_depth);
		record_keys(t, t->nil, node_arr, node_idx_list, cur_depth + 1, render_depth);
	}
	else {
		record_keys(t, p->left, node_arr, node_idx_list, cur_depth + 1, render_depth);
		record_keys(t, p->right, node_arr, node_idx_list, cur_depth + 1, render_depth);
	}
}

void visualize_tree(rbtree* t, int render_depth) {
	// 트리 시각화 함수
	// depth는 루트를 0으로 계산
	// key_arr[i][j]: 깊이가 i인 (j+1)번째 원소의 키 **2차원 배열 접근이 불가능해 1차원 배열로 구현
	node_t** node_arr = calloc((render_depth + 1) * pow_2(render_depth), sizeof(node_t));

	// key_idx_list[i]: key_arr[i]의 현재 인덱스
	int* node_idx_list = (int*)calloc(render_depth + 1, sizeof(int));

	char* filler = "  ";

	// 키를 배열에 기록
	record_keys(t, t->root, node_arr, node_idx_list, 0, render_depth);

	// 기록된 키를 출력
	for (int i = 0; i < render_depth + 1; i++) {
		for (int j = 0; j < pow_2(i); j++) {
			for (int k = 0; k < pow_2(render_depth - i); k++) { // front buffer
				printf("%s", filler);
			}
			if (node_arr[i * pow_2(render_depth) + j] == t->nil) {
				printf("\x1b[0m" "%2s", ".");
			}
			else if (node_arr[i * pow_2(render_depth) + j]->color == RBTREE_BLACK) {
				// 검정 노드는 흰색으로 출력
				printf("\x1b[0m" "%2d", node_arr[i * pow_2(render_depth) + j]->key);
			}
			else {
				// 빨강 노드는 빨간색으로 출력
				printf("\x1b[31m" "%2d", node_arr[i * pow_2(render_depth) + j]->key);
			}
		}
		printf("\n");
	}
	printf("\n");

	free(node_arr);
	free(node_idx_list);
}
////


rbtree* new_rbtree(void) {
	rbtree* p = (rbtree*)calloc(1, sizeof(rbtree));

	// calloc 실패 시
	if (p == NULL) {
		printf("메모리 할당 거부됨.\n");
		return NULL;
	}

	// 트리의 구성요소 메모리 할당
	//// nil 노드 정의
	p->nil = new_node();
	p->nil->color = RBTREE_BLACK;
	p->nil->left = NULL;
	p->nil->right = NULL;
	//// root 노드 정의
	p->root = p->nil;
	p->root->parent = p->nil;

	return p;
}

void delete_rbtree(rbtree* t) {
	if (t == NULL)
		return;

	clear_tree(t, t->root);
	free(t->nil);
	free(t);
}

node_t* rbtree_insert(rbtree* t, const key_t key) {
	if (t == NULL)
		t = new_rbtree();

	node_t* new_node = node_insert(t, &(t->root), t->nil, key);
	
	// rbtree 규칙 검사 및 수정 함수
	rb_insert_fixup(t, new_node);
	rootBlack(t);

	return t->root;
}

node_t* rbtree_find(const rbtree* t, const key_t key) {
	if (t == NULL)
		t = new_rbtree();

	return node_find(t, t->root, key, 0);
}

node_t* rbtree_min(const rbtree* t) {
	if (t == NULL)
		t = new_rbtree();

	return node_min(t, t->root);
}

node_t* rbtree_max(const rbtree* t) {
	if (t == NULL)
		t = new_rbtree();

	return node_max(t, t->root);
}

int rbtree_erase(rbtree* t, node_t* p) {
	if (t == NULL)
		t = new_rbtree();

	if (p == NULL) {
		printf("삭제할 노드 정의 잘못됨!\n");
		return -1;
	}

	node_t* swap_node = p;
	node_t* x;
	color_t origin_col = swap_node->color;
	if (p->left == t->nil) {
		x = p->right;
		rbtree_transplant(t, p, p->right);
	}
	else if (p->right == t->nil) {
		x = p->left;
		rbtree_transplant(t, p, p->left);
	}
	else {
		swap_node = node_min(t, p->right);
		origin_col = swap_node->color;
		x = swap_node->right;
		if (swap_node->parent == p)
			x->parent = swap_node;
		else {
			rbtree_transplant(t, swap_node, swap_node->right);
			swap_node->right = p->right;
			swap_node->right->parent = swap_node;
		}
		rbtree_transplant(t, p, swap_node);
		swap_node->left = p->left;
		swap_node->left->parent = swap_node;
		swap_node->color = p->color;
	}

	if (origin_col == RBTREE_BLACK)
		rb_delete_fixup(t, x);
}

int rbtree_to_array(const rbtree* t, key_t* arr, const size_t n) {
	if (t == NULL)
		t = new_rbtree();
	if (arr == NULL) {
		printf("배열 정의 잘못됨!\n");
		return -1;
	}

	make_inorder_array(t, arr, t->root, 0);
	return 0;
}



///////////////////////////////////
//int rbtree_erase(rbtree* t, node_t* p) {
//	if (t == NULL)
//		t = new_rbtree();
//
//	if (p == NULL) {
//		printf("삭제할 노드 정의 잘못됨!\n");
//		return -1;
//	}
//
//	// 삭제할 노드의 오른쪽 자식이 하나라도 삽입되어 있다면
//	if (p->right != t->nil) {
//		// swap_node: 큰 놈중에 작은 애
//		node_t* swap_node = node_min(t, p->right);
//
//		// 스왑 노드가 오른쪽으로 뻗어나간 애라면
//		if (swap_node == swap_node->parent->right) {
//			swap_node->right->parent = swap_node->parent;
//			swap_node->parent->right = swap_node->right;
//		}
//		// 스왑 노드가 왼쪽으로 뻗어나간 애라면
//		else {
//			// 스왑 노드의 오른쪽 자식이 하나라도 삽입되어 있다면
//			if (swap_node->right != t->nil) {
//				swap_node->right->parent = swap_node->parent;
//				swap_node->parent->left = swap_node->right;
//			}
//			else
//				swap_node->parent->left = swap_node->left;
//		}
//
//		p->key = swap_node->key;
//		p = swap_node;
//	}
//	// 삭제할 노드의 왼쪽 자식만 있다면
//	else if (p->left != t->nil) {
//		p->left->parent = p->parent;
//		// 삭제할 노드가 루트라면
//		if (p == t->root) {
//			p->left->parent = t->nil;
//			t->root = p->left;
//		}
//		else
//			p->parent->left = p->left;
//		p->left->color = p->color;
//	}
//	// 삭제할 노드가 자식이 없는 노드라면
//	else {
//		// 삭제할 노드가 루트라면
//		if (p == t->root)
//			t->root = t->nil;
//		// 왼쪽 말단 노드라면
//		else if (p == p->parent->left)
//			p->parent->left = p->left;
//		// 오른쪽 말단 노드라면
//		else
//			p->parent->right = p->right;
//	}
//
//	// rbtree 규칙 검사 및 수정 함수
//	if (p->color == RBTREE_RED)
//		rb_delete_fixup(t, p);
//	else if (p->parent->left == t->nil && p->parent->right != t->nil) {
//		t->nil->parent = p->parent;
//		rb_delete_fixup(t, t->nil);
//	}
//	else if (p->parent->right == t->nil && p->parent->left != t->nil) {
//		t->nil->parent = p->parent;
//		rb_delete_fixup(t, t->nil);
//	}
//	else
//		rb_delete_fixup(t, p);
//
//	rootBlack(t);
//	free(p);
//
//	return 0;
//}