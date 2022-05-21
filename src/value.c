#include <stdio.h>
#include "value.h"

value_t *create_value(type_t type,int size){
  
  value_t *v;
  v = mem(sizeof(value_t));
  v->obj.type = TYPE_VALUE;
  v->obj.scope = UNDEFINED;
  v->obj.next = NULL;
  v->type = type;
  v->size = size;

  return v;
}
