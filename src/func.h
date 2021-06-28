#ifndef __INCLUDE__FUNC__
#define __INCLUDE__FUNC__
#include "type.h"
#include "obj.h"
#include "list.h"

#define FUNC_GET_RET_TYPE(f) f->ret_type

struct func_t {
  object_t obj;
  list_t *args;
  list_t *ret_type;
  int len;
};

func_t *create_func(list_t *args,list_t *ret_type,scope_t scope);

#endif
