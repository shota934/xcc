#ifndef  __INCLUDE__OBJ__
#define __INCLUDE__OBJ__
#include "type.h"

struct object_t {
  type_type_t type;
  scope_t scope;
};

#define OBJ_GET_TYPE(o)      o->type
#define OBJ_GET_SCOPE(o)     o->scope

#endif
