#ifndef __INCLUDE__FUNC__
#define __INCLUDE__FUNC__
#include "type.h"
#include "obj.h"
#include "list.h"

#define FUNC_GET_TYPE(f)     f->obj.type
#define FUNC_GET_RET_TYPE(f) f->ret_type
#define FUNC_GET_NO_VAR_ARG(f) f->no_var_args
#define FUNC_IS_HAVING_VAR_ARG(f) f->has_var_args
#define FUNC_GET_NUM_OF_ARGS(f)   f->num_of_args;
#define FUNC_GET_NAME(f)          f->name

struct func_t {
  object_t obj;
  list_t *args;
  list_t *no_var_args;
  list_t *ret_type;
  bool_t has_var_args;
  int num_of_args;
  string_t name;
};

func_t *create_func(list_t *args,list_t *ret_type,scope_t scope,string_t name);

#endif
