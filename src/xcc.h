// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__XCC__
#define __INCLUDE__XCC__
#include "type.h"

#define SET_PREPROCESS(oi,f)    oi->only_preprocess = f
#define SET_GENASSEMBLER(oi,f)  oi->only_genassembler = f
#define SET_GEN_OBJFILE(oi,f)   oi->only_gen_objfile = f

// built-in macro
#define X86_64       "__x86_64__"
#define LP64         "__LP64__"
#define LINUX        "__linux__"
#define STDC_VERSION "__STDC_VERSION__"

#endif
