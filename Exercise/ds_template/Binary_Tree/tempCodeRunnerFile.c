        if(s_t->top == NULL){
            return 0;
        }
        BTNode *new_t = pop(s_t);
        if(new->item != new_t->item){
            return 0;
        }
    }

    if(s_t->top != NULL){
        return 0;
    }

    return 1;
   }

   if(tree1 == NULL && tree2 == NULL){
    return 1;
   }

   return 0;
}