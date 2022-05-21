#ifndef __INCLUDE__OPE__
#define __INCLUDE__OPE__
#include "obj.h"
#include "type.h"

#define OPE_GET_TYPE(ope) ope->type
#define OPE_GET_SIZE(ope) ope->size
#define OPE_SET_SRUT_OFFSET(ope,os) ope->srut.offset = os
#define OPE_GET_SRUT_OFFSET(ope) ope->srut.offset
#define OPE_SET_OP(ope,op1) ope->op = op1
#define OPE_GET_OP(ope) ope->op

#define OPE_SET_TARGET(ope,obj) ope->target.obj = obj
#define OPE_GET_TARGET(ope) ope->target.obj

struct operator_t {
  object_t obj;
  type_t type;
  int size;
  operation_t op;

  struct {
	int offset;
  }srut;

  struct {
	object_t *obj;
  } target;
};

operator_t *create_operator(type_t type,int size);

#endif
