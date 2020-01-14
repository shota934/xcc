// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__STATCK__
#define __INCLUDE__STATCK__
#include "type.h"

struct stack_ty {
  list_t *lst;
  int size;
};

stack_ty *create_stack();
void push(stack_ty *stack,void *obj);
void *pop(stack_ty *stack);
void *top(stack_ty *stack);
int empty(stack_ty *stack);
int get_size(stack_ty *stack);

#endif

