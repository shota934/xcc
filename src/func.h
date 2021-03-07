#ifndef __INCLUDE__FUNC__
#define __INCLUDE__FUNC__
#include "type.h"
#include "obj.h"
#include "list.h"

struct func_t {
  object_t obj_type;
  list_t *args;
  int len;
  list_t *ret_type;
};

func_t *create_func();

#endif
