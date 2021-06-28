#include "func.h"
#include "mem.h"
#include "type.h"

func_t *create_func(list_t *args,list_t *ret_type,scope_t scope){

  func_t *func;

  func = mem(sizeof(func_t));
  func->obj.type = TYPE_FUNCTION;
  func->obj.scope = scope;
  func->args = args;
  func->ret_type = ret_type;
  func->len = length_of_list(args);

  return func;
}
