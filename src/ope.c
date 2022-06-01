#include <stdio.h>
#include "ope.h"
#include "mem.h"

operator_t *create_operator(type_t type,int size){

  operator_t *ope;

  ope = mem(sizeof(operator_t));
  ope->obj.type = TYPE_OPE;
  ope->obj.scope = UNDEFINED;
  ope->obj.next = NULL;
  ope->type = type;
  ope->size = size;
  ope->srut.offset = 0;

  return ope;
}

void operator_set_target_obj(operator_t *ope,object_t *obj){

  ope->target_obj = obj;

  return;
}

object_t *operator_get_target_obj(operator_t *ope){
  return ope->target_obj;
}
