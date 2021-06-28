//
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "compound_type.h"
#include "mem.h"

compound_def_t *create_compound_type_def(type_type_t type,scope_t scope){

  compound_def_t *cdt;
  
  cdt = mem(sizeof(compound_def_t));
  cdt->obj_type.type = type;
  cdt->obj_type.scope = scope;
  cdt->type = OTHER_COMPOUND_TYPE;
  cdt->size = 0;
  
  return cdt;
}
