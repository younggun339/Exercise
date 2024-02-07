#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

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

static void visualize_tree(rbtree *t, int render_depth) {
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


void replace_node(rbtree *t, node_t *del_target, node_t *swap_target)
{
  //삭제할 타겟의 부모가 nil 노드 라면?  -> root 라는 얘기겠죵?
  if (del_target->parent == t->nil)
      t->root = swap_target; // 루트에 삭제될 자리에 올 타겟 넣어주고
  //삭제할 타겟이랑 삭제할타겟의 부모의 왼쪽자식이랑 같다면 
  else if (del_target == del_target->parent->left)
      del_target->parent->left = swap_target;//삭제할 타겟의 부모의 왼쪽에 대체할놈
  else
      del_target->parent->right = swap_target;//반대로 오른쪽의 경우
  
  //대체할놈의 부모를 삭제할놈의 부모로 설정
  swap_target->parent = del_target->parent;
  
}


void *erase_binary(rbtree *t, node_t *del_target)
{
  node_t* extra_black=del_target;// RB특성을 위반할 수 있는 노드를 관리하기위한 변수 = 청구서, extra블랙
  node_t* swap_target; // del_target의 원래 위치에 올 녀석도 RB트리 특성 위반할수있기때문에 만들어놓고 확인
  color_t successor; // 원래있던자리의 색을 확인하기위한 변수
  successor = extra_black->color;


  // del_target 자식에따라 바뀜
  if (del_target->left == t->nil){  // 오른쪽만있을때
      swap_target=del_target->right; 
      replace_node(t,del_target,del_target->right); 
  }
  else if (del_target->right == t->nil){ // 왼쪽만 있을때
      swap_target=del_target->left;      
      replace_node(t,del_target,del_target->left); 
  }else{                                        // 양쪽 다 있을때
      extra_black=tree_minimum(t,del_target->right); 
      successor=extra_black->color;
      swap_target=extra_black->right;      
      // 양쪽다 자식이있고 삭제할놈 바로밑에서 대체될놈을 가져올때         
      if (extra_black->parent ==del_target &&    
          extra_black->right !=t->nil){            
          swap_target->parent=extra_black;         
      }else{                                       
          replace_node(t,extra_black,extra_black->right); 
          extra_black->right=del_target->right;         
          extra_black->right->parent=extra_black;          
      }                                 
      replace_node(t,del_target,extra_black);     
      extra_black->left=del_target->left;         
      extra_black->left->parent=extra_black;        
      extra_black->color=del_target->color;       
  }

  if (successor == RBTREE_BLACK){             // 삭제할놈의 오리지널 컬러가 블랙이라면?
      delete_fixup(t,swap_target);
  }
  free(del_target);

}


void delete_fixup(rbtree* t,node_t* x)
{
  node_t* brother;
   while (x!=t->root && x->color == RBTREE_BLACK)
   {
        if(x == x->parent->left){  // left 일때
              brother=x->parent->right; // 부라더 설정
              // case  1 : 형제가 RED 일때
              if (brother->color==RBTREE_RED){
                  brother->color=RBTREE_BLACK;
                  brother->parent->color=RBTREE_RED;
                  rotate_left(t,brother->parent);
                  brother=x->parent->right;
              }

              // case 2 : 형제의 양쪽 자식이 다 BLACK 일 떄
              if (brother->left->color == RBTREE_BLACK &&
                  brother->right->color == RBTREE_BLACK){
                    brother->color=RBTREE_RED;
                    x=x->parent;
                  }
              // case 3 : 형제의 오른쪽 자식이 BLACK 일 때
              else {
                if (brother->right->color == RBTREE_BLACK){
                  brother->left->color = RBTREE_BLACK;
                  brother->color = RBTREE_RED;
                  rotate_right(t,brother);
                  brother = x->parent->right; 
                }
                // case 4 : 형제의 왼쪽 자식이 BLACK 일 때
                brother->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                brother->right->color = RBTREE_BLACK;
                rotate_left(t,x->parent);
                x=t->root;
              }
        } 
        else{ // right 일떄
               brother=x->parent->left; // 부라더 설정
              // case  1 : 형제가 RED 일때
              if (brother->color==RBTREE_RED){
                  brother->color=RBTREE_BLACK;
                  brother->parent->color=RBTREE_RED;
                  rotate_right(t,brother->parent);
                  brother=x->parent->left;
              }

              // case 2 : 형제의 양쪽 자식이 다 BLACK 일 떄
              if (brother->right->color == RBTREE_BLACK &&
                  brother->left->color == RBTREE_BLACK){
                    brother->color=RBTREE_RED;
                    x=x->parent;
                  }
              // case 3 : 형제의 오른쪽 자식이 BLACK 일 때
              else {
                if (brother->left->color == RBTREE_BLACK){
                    brother->right->color = RBTREE_BLACK;
                    brother->color = RBTREE_RED;
                    rotate_left(t,brother);
                    brother = x->parent->left; 
                  }
                  // case 4 : 형제의 왼쪽 자식이 BLACK 일 때
                  brother->color = x->parent->color;
                  x->parent->color = RBTREE_BLACK;
                  brother->left->color = RBTREE_BLACK;
                  rotate_right(t,x->parent);
                  x=t->root;
              }
        }
   }
   x->color=RBTREE_BLACK;
}

// void delete_node(rbtree *t, node_t *n)
// {
//     node_t *child = (n->right == t->nil) ? n->left : n->right;
//     if (n == t->root)
//         return;
//     replace_node(t, n, child);
    
//     // Debugging messages
//     printf("n->color: %d, child->color: %d\n", n->color, child->color);
//     if (n->color == RBTREE_BLACK)
//     {
//         printf("if 문 들어오나 ?\n");
//         if (child->color == RBTREE_RED)
//         {
//             child->color = RBTREE_BLACK;
//             printf("child->color set to RBTREE_BLACK\n");
//         }
//         else
//         { // case 1:
//             // printf("fixed 호출\n");
//             erase_fixed(t, child);
//             // printf("fixed 호출\n");
//             visualize_tree(t,4);
            
//         }
//     }
//     free(n);
// }


void erase_fixed(rbtree *t, node_t *n)
{

   if (n->parent == t->nil){
        return;
    }
    node_t *s = get_sibling(t, n);

    // case 2:
    if (s->color == RBTREE_RED)
    {
        printf("erase_fixed: case 2 들어오니?\n");
        n->parent->color = RBTREE_RED;
        s->color = RBTREE_BLACK;

        if (n == n->parent->left)
            rotate_left(t, n->parent);
        else
            rotate_right(t, n->parent);
    }
    // case 3:
    else if ((n->parent->color == RBTREE_BLACK) &&
             (s->color == RBTREE_BLACK) &&
             (s->left->color == RBTREE_BLACK) &&
             (s->right->color == RBTREE_BLACK))
    {
        printf("erase_fixed: case 3 들어오니?\n");
        s->color = RBTREE_RED;
        erase_fixed(t, n->parent);
    }
    // case 4:
    else if ((n->parent->color == RBTREE_RED) &&
             (s->color == RBTREE_BLACK) &&
             (s->left->color == RBTREE_BLACK) &&
             (s->right->color == RBTREE_BLACK))
    {
        printf("erase_fixed: case 4 들어오니?\n");
        s->color = RBTREE_RED;
        n->parent->color = RBTREE_BLACK;
    }
    // case 5:
    else if (s->color == RBTREE_BLACK)
    {
        if ((n == n->parent->left) &&
            (s->right->color == RBTREE_BLACK) &&
            (s->left->color == RBTREE_RED))
        {
            printf("erase_fixed: case 5 들어오니?\n");
            s->color = RBTREE_RED;
            s->left->color = RBTREE_BLACK;
            rotate_right(t, s);
        }
        else if ((n == n->parent->right) &&
                 (s->left->color == RBTREE_BLACK) &&
                 (s->right->color == RBTREE_RED))
        {
            s->color = RBTREE_RED;
            s->right->color = RBTREE_BLACK;
            rotate_left(t, s);
        }
    }
    // case 6:
    else
    {
        printf("erase_fixed: case 6 들어오니?\n");
        s->color = n->parent->color;
        n->parent->color = RBTREE_BLACK;

        if (n == n->parent->left)
        {
            s->right->color = RBTREE_BLACK;
            rotate_left(t, n->parent);
        }
        else
        {
            s->left->color = RBTREE_BLACK;
            rotate_right(t, n->parent);
        }
    }
}






node_t *tree_minimum(rbtree *t, node_t *x)
{
    while (x->left != t->nil)
        x = x->left;

    return x;
}
