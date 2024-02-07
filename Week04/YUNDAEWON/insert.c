#include "rbtree.h"
#include <stdio.h>
void insert_fixed(rbtree *t,node_t *n){
    node_t *u=get_uncle(t,n);
    node_t *g=get_grandparent(t,n);

    //case 1: 삽입한 노드가 루트일때
    if(t->root == t->nil){
        t->root=n;
        n->color=RBTREE_BLACK;
    } else {
        //case 2: 삽입한 노드의 부모의 색이 블랙이면 통과
        if (n->parent->color==RBTREE_BLACK)
            return;
        //case 3: 삽입한노드의 부모가 RED and 삼촌도 red 면 부모,삼촌 BLACK change
        // 할아버지도 red로 바꾸고 다음 규칙이 깨졌는지 재귀적으로 확인(할아버지의 부모가 있다면 rbtree의 5번속성이 깨지기때문에)
        else{
            if ((u!=NULL) && (u->color==RBTREE_RED)){
            n->parent->color=RBTREE_BLACK;
            u->color=RBTREE_BLACK;
            g->color=RBTREE_RED;
            insert_fixed(t,g);
            }
            else {
            // case 4:
                if ((n==n->parent->right) && (n->parent==g->left))
                {
                    rotate_left(t, n->parent);
                    n=n->left;
                } else if((n==n->parent->left) && (n->parent == g->right)){
                    rotate_right(t, n->parent);
                    n=n->right;
                }

            // case 5:
            if (g != t->nil) {
                n->parent->color = RBTREE_BLACK;
                g->color = RBTREE_RED;
                if (n == n->parent->left)
                    rotate_right(t,g);
                else
                    rotate_left(t,g);
                }
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}   


void insert_binary(rbtree *t, node_t *n)
{
    node_t *next= t->root;
    node_t *parent= t->nil;

    if (t->root != t->nil){

        while (next != t->nil)
        {
            parent = next;

            if(n->key < next->key)
                next = next->left;
            else
                next = next->right;
        }

        n->parent=parent;

        if (n->key < parent->key){
            parent->left = n;
        } else {
            parent->right = n;
        }
    }
}
