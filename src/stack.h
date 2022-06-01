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
void stack_push(stack_ty *stack,void *obj);
void *stack_pop(stack_ty *stack);
void *stack_top(stack_ty *stack);
int stack_empty(stack_ty *stack);
int stack_get_size(stack_ty *stack);

#endif

