#include "func.h"
#include "mem.h"
#include "type.h"

static list_t *make_type_list_of_variable(list_t *args);

func_t *create_func(list_t *args,list_t *ret_type,scope_t scope){

  func_t *func;

  func = mem(sizeof(func_t));
  func->obj.type = TYPE_FUNCTION;
  func->obj.scope = scope;
  func->args = args;
  func->no_var_args = make_type_list_of_variable(args);
  func->has_var_args = FALSE;
  func->ret_type = ret_type;
  func->len = length_of_list(args);

  return func;
}

static list_t *make_type_list_of_variable(list_t *args){

  list_t *l;

  if(IS_NULL_LIST(args)){
	return make_null();
  } else {
	l = car(args);
	l = add_list(make_null(),cdr(l));
	return concat(l,make_type_list_of_variable(cdr(args)));
  }
}
