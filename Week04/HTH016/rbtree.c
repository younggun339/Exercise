#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>





// RB트리 생성
rbtree* new_rbtree(void) {
    // 트리 구조체 생성
    rbtree* t = (rbtree*)calloc(1, sizeof(rbtree));
    // node 구조체 nil로 초기화하여 생성
    node_t* nil = (node_t*)malloc(sizeof(node_t));
    nil->color = RBTREE_BLACK;
    //nil->key = NULL;

    t->root = nil;
    t->nil = nil;
    /*
    t->nil->parent = t->nil;
    t->nil->left = t->nil;
    t->nil->right = t->nil;
    */
    return t;
}

// RB트리 삭제
void delete_rbtree(rbtree* t) {

    if (t->root != t->nil) {
        // 후위 순위 재귀적으로 삭제 (insert 개수 - erase 개수만큼)
        recursive_delete_rbtree(t, t->root);
    }
    // nil 구조체 삭제
    free(t->nil);
    // 트리 구조체 삭제
    free(t);
}

// 후위 순위 메모리 해제 
void recursive_delete_rbtree(rbtree* t, node_t* node) {
    if (node->left != t->nil && node != t->nil) {
        recursive_delete_rbtree(t, node->left);
    }
    if (node->right != t->nil && node != t->nil) {
        recursive_delete_rbtree(t, node->right);
    }
    free(node);
}





// RB-TREE-LEFT-ROTATE
void left_rotate(rbtree* t, node_t* to_left) {
    node_t* to_center = to_left->right;
    to_left->right = to_center->left;
    if (to_center->left != t->nil) {
        to_center->left->parent = to_left;
    }
    to_center->parent = to_left->parent;
    if (to_left->parent == t->nil) {
        t->root = to_center;
    }
    else if (to_left == to_left->parent->left) {
        to_left->parent->left = to_center;
    }
    else {
        to_left->parent->right = to_center;
    }
    to_center->left = to_left;
    to_left->parent = to_center;
}

// RB-TREE-RIGHT-ROTATE
void right_rotate(rbtree* t, node_t* to_right) {
    node_t* to_center = to_right->left;
    to_right->left = to_center->right;

    if (to_center->right != t->nil) {
        to_center->right->parent = to_right;
    }
    to_center->parent = to_right->parent;
    if (to_right->parent == t->nil) {
        t->root = to_center;
    }
    else if (to_right == to_right->parent->right) {
        to_right->parent->right = to_center;
    }
    else {
        to_right->parent->left = to_center;
    }
    to_center->right = to_right;
    to_right->parent = to_center;
}





void rbtree_insert_fix_up(rbtree* t, node_t* target) {
    node_t* uncle;
    while (target->parent->color == RBTREE_RED) {
        if (target->parent == target->parent->parent->left) {
            uncle = target->parent->parent->right;
            if (uncle->color == RBTREE_RED) {
                target->parent->color = RBTREE_BLACK;
                uncle->color = RBTREE_BLACK;
                target->parent->parent->color = RBTREE_RED;
                target = target->parent->parent;
            }
            else {
                if (target == target->parent->right) {
                    target = target->parent;
                    left_rotate(t, target);
                }
                target->parent->color = RBTREE_BLACK;
                target->parent->parent->color = RBTREE_RED;
                right_rotate(t, target->parent->parent);
            }
        }
        else {
            uncle = target->parent->parent->left;
            if (uncle->color == RBTREE_RED) {
                target->parent->color = RBTREE_BLACK;
                uncle->color = RBTREE_BLACK;
                target->parent->parent->color = RBTREE_RED;
                target = target->parent->parent;
            }
            else {
                if (target == target->parent->left) {
                    target = target->parent;
                    right_rotate(t, target);
                }
                target->parent->color = RBTREE_BLACK;
                target->parent->parent->color = RBTREE_RED;
                left_rotate(t, target->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}




node_t* rbtree_insert(rbtree* t, const key_t key) {
    node_t* recorder = t->nil;
    node_t* node = t->root;
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->key = key;

    while (node != t->nil) {
        recorder = node;
        if (new_node->key < node->key) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    new_node->parent = recorder;
    if (recorder == t->nil) {
        t->root = new_node;
    }
    else if (new_node->key < recorder->key) {
        recorder->left = new_node;
    }
    else {
        recorder->right = new_node;
    }
    new_node->left = t->nil;
    new_node->right = t->nil;
    new_node->color = RBTREE_RED;

    rbtree_insert_fix_up(t, new_node);
    return t->root;
}

// RB트리 탐색
node_t* rbtree_find(const rbtree* t, const key_t key) {

    node_t* node = t->root;
    if (t->root == t->nil) {
        return NULL;
    }
    while (node != t->nil) {
        if (node->key == key) {
            return node;
        }
        else if (node->key > key) {
            node = node->left;
        }
        else if (node->key < key) {
            node = node->right;
        }
        else if (node->left == t->nil && node->right == t->nil) {
            return NULL;
        }
        else
            break;
    }
    return NULL;
}

// RB트리 최솟값
node_t* rbtree_min(const rbtree* t) {
    node_t* node = t->root;

    while (node->left != t->nil) {

        node = node->left;
    }
    return node;
}

// RB트리 최댓값
node_t* rbtree_max(const rbtree* t) {
    node_t* node = t->root;

    while (node->right != t->nil)
        node = node->right;
    return node;
}


node_t* rbtree_successor(const rbtree* t, node_t* pre) 
{
    node_t* node = pre;
    if (node == t->nil || node->right == t->nil) {
        return NULL;
    }
    else {

        while (node->right != t->nil)
        {
            node = node->right;
        }
        return node;
    }
}

node_t* subtree_min(const rbtree* t, node_t* right) 
{
    node_t* x = NULL;
    x = right;
    while (x->left != t->nil)
    {
        x = x->left;
    }
    return x;
}


int rbtree_erase(rbtree* t, node_t* target){
    node_t* be_fixed;
    node_t* relocated = target;
    color_t deleted_color = relocated->color;
    if (target->left == t->nil){
        be_fixed = target->right;
        trans_plant(t, target, target->right);
    }
    else if (target->right == t->nil){
        be_fixed = target->left;
        trans_plant(t, target, target->left);
    } else {
        relocated = subtree_min(t, target->right);
        deleted_color = relocated->color;
        be_fixed = relocated->right;
        if (relocated->parent == target) {
            be_fixed->parent = relocated;
        } else {
            trans_plant(t, relocated, relocated->right);
            relocated->right = target->right;
            relocated->right->parent = relocated;
        }
        trans_plant(t, target, relocated);
        relocated->left = target->left;
        relocated->left->parent = relocated;
        relocated->color = target->color;
        
    }
    free(target);
    if (deleted_color == RBTREE_BLACK)
        rbtree_delete_fix_up(t, be_fixed);

    return 0;
}

void rbtree_delete_fix_up(rbtree* t, node_t* target) {
    node_t* sibling;
    while (target != t->root && target->color == RBTREE_BLACK) {
        if (target == target->parent->left) {
            sibling = target->parent->right;
            if (sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                target->parent->color = RBTREE_RED;
                left_rotate(t, target->parent);
                sibling = target->parent->right;
                
            }
            else if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
                sibling->color = RBTREE_RED;
                target = target->parent;
                
            }
            else {

                if (sibling->right->color == RBTREE_BLACK) {
                    sibling->left->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    right_rotate(t, sibling);
                    sibling = target->parent->right;
                }
                sibling->color = target->parent->color;
                target->parent->color = RBTREE_BLACK;
                sibling->right->color = RBTREE_BLACK;
                left_rotate(t, target->parent);
                target = t->root;
            }
            
        }
        else {
            sibling = target->parent->left;
            if (sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                target->parent->color = RBTREE_RED;
                right_rotate(t, target->parent);
                sibling = target->parent->left;
                
            }
            else if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
                sibling->color = RBTREE_RED;
                target = target->parent;
                
            }
            else {
                if (sibling->left->color == RBTREE_BLACK) {
                    sibling->right->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    left_rotate(t, sibling);               
                    sibling = target->parent->left;
                }
                sibling->color = target->parent->color;
                target->parent->color = RBTREE_BLACK;
                sibling->left->color = RBTREE_BLACK;
                right_rotate(t, target->parent);
                target = t->root;
            }
            
        }
    }
    target->color = RBTREE_BLACK;
    // t->root->color = RBTREE_BLACK;
}

// TRANSPLANT
void trans_plant(rbtree* t, node_t* old_node, node_t* new_node) {
    if (old_node->parent == t->nil) {
        t->root = new_node;
    }
    else if (old_node == old_node->parent->left) {
        old_node->parent->left = new_node;
    }
    else {
        old_node->parent->right = new_node;
    }
    new_node->parent = old_node->parent;
}

void inorder_search(node_t* root, node_t* nil, key_t* arr, int* idx) {
    if (root == nil) 
        return;
    inorder_search(root->left, nil, arr, idx);
    arr[(*idx)++] = root->key;
    inorder_search(root->right, nil, arr, idx);
}

int rbtree_to_array(const rbtree* t, key_t* arr, const size_t n) {
    int* idx = (int*)calloc(1, sizeof(int));
    inorder_search(t->root, t->nil, arr, idx);
    free(idx);
    return 0;
}
