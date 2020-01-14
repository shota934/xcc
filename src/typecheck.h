// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__TYPECHECK__
#define __INCLUDE__TYPECHECK__
#include "list.h"
#include "env.h"

bool_t check_floattype(env_t *env,list_t *l,list_t *r);
bool_t check_type_lst(list_t *lst1,list_t *lst2);


#endif
