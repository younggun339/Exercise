#include "rbtree.h"
#include <stdlib.h>


//할아버지 노드 가져오는 함수
node_t *get_grandparent(rbtree *t,node_t *n)
{
  if ((n!=t->nil) && (n->parent!=t->nil))
    return n->parent->parent;
  else
    return t->nil;
}
//삼촌 노드 가져오는 함수
node_t *get_uncle(rbtree *t, node_t *n)
{
  node_t *g=get_grandparent(t,n);
  if(g==t->nil)
    return t->nil;
  if (n->parent==g->left)
    return g->right;
  else
    return g->left;
}

//형제 노드 가져오기
node_t *get_sibling(rbtree*t, node_t *n)
{
    if (n == t->nil || n->parent == t->nil)
        return t->nil;

    node_t *parent = n->parent;

    if (n == parent->left)
        return parent->right;
    else
        return parent->left;
}

//로테이트 레프트
void rotate_left(rbtree *t, node_t *n)
{
    node_t *c=n->right;
    node_t *p=n->parent;

    if (c->left!=t->nil)
        c->left->parent=n;
    
    n->right = c->left;
    n->parent = c;
    c->left = n;
    c->parent = p;

    if (p!=t->nil){
        if(p->left == n)
            p->left = c;
        else
            p->right = c;
    } else {
		t->root = c;
	}
}

void rotate_right(rbtree *t, node_t *n)
{
    node_t *c=n->left;
    node_t *p=n->parent;

    if (c->right != t->nil)
        c->right->parent = n;
    
    n->left = c->right;
    n->parent = c;
    c->right = n;
    c->parent = p;

    if (p != t->nil){
        if (p->right ==n)
            p->right = c;
        else
            p->left = c;
    } else {
		t->root = c;
	}
}

void free_inorder_traversal(rbtree *t, node_t *root)
{
    if (root != t->nil){
        free_inorder_traversal(t,root->left);
        free(root);
        free_inorder_traversal(t,root->right);
    }
}

void to_arr_inorder_traversal(rbtree *t, node_t *root, key_t *arr, size_t *index)
{
    if (root != t->nil){
        to_arr_inorder_traversal(t, root->left, arr, index);
        arr[(*index)++] = root->key; // 현재 노드의 키 값을 배열에 저장
        to_arr_inorder_traversal(t, root->right, arr, index);
    }
}



