#ifndef  __INCLUDE__OBJ__
#define __INCLUDE__OBJ__
#include "type.h"

struct object_t {
  type_type_t type;
  scope_t scope;
  object_t *next;
};

#define OBJ_GET_TYPE(o)      o->type
#define OBJ_GET_SCOPE(o)     o->scope
#define OBJ_SET_NEXT(o1,o2)  o1->next = o2
#define OBJ_GET_NEXT(o1)  o1->next

type_type_t get_obj_type(object_t *obj);

#endif
