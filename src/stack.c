// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "stack.h"
#include "mem.h"
#include "common.h"
#include "list.h"

stack_ty *create_stack(){

  stack_ty *stack = mem(sizeof(stack_ty));
  stack->lst = make_null();
  stack->size = 0;
  
  return stack;
}

void push(stack_ty *stack,void *obj){

  stack->lst = cons(stack->lst,obj);
  stack->size++;
  
  return;
}

void *pop(stack_ty *stack){

  void *obj;
  if(IS_NULL_LIST(stack->lst)){
    obj = NULL;
  } else {
    obj = car(stack->lst);
    stack->lst = cdr(stack->lst);
    stack->size--;
  }
  
  return obj;
}

void *top(stack_ty *stack){

  void *obj;

  if(IS_NULL_LIST(stack->lst)){
    obj = NULL;
  } else {
    obj = car(stack->lst);
  }

  return obj;
}

int empty(stack_ty *stack){
  return IS_NULL_LIST(stack->lst);
}

int get_size(stack_ty *stack){
  return stack->size;
}
