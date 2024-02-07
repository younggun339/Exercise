#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;
//null이 아니라 nil을 쓰는 이유는 모드 리프가 하나의 NULL값으로 가질때
//메모리 공간을 크게 낭비하기 때문에 그런거임.
//그리고 nil노드를 써서 리프를 나타내면 모든 리프가 하나의 nil을 가지게 되서 메모리공간을 절약함.
typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);
//헤더 파일에 함수를 안넣으려고 여기다가 배치함. 
void traverse_and_delete_node(rbtree *t, node_t *node);
void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
node_t *get_node(const rbtree *t, node_t *p);
void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left);
void exchange_color(node_t *a, node_t *b);


int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
