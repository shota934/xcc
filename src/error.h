// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef  __INCLUDE__ERROR__
#define  __INCLUDE__ERROR__
#include "type.h"

void error(int line,string_t name,string_t msg,...);
void error_no_info(string_t msg,...);

#endif
