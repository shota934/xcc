// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "compound_type.h"

#include "mem.h"

compound_def_t *create_compound_type_def(){

  compound_def_t *cdt;
  
  cdt = mem(sizeof(compound_def_t));
  cdt->type = OTHER_COMPOUND_TYPE;
  cdt->members = make_null();
  cdt->size = 0;
  
  return cdt;
}
