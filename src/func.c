#include "func.h"
#include "mem.h"
#include "type.h"

func_t *create_func(list_t *args,list_t *ret_type){

  func_t *func;

  func = mem(sizeof(func_t));
  func->obj_type.type = TYPE_FUNCTION;
  func->args = args;
  func->ret_type = ret_type;
  func->len = length_of_list(args);
  
  return func;
}
