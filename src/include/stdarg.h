// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __SYDARG__
#define __SYDARG__

typedef struct {
  unsigned int gp_offset;
  unsigned int fp_offset;
  void *overflow_arg_area;
  void *reg_save_area;
} __va_elem;

typedef __va_elem va_list[1];

#define __GNUC_VA_LIST 1

typedef va_list __gnuc_va_list;

#define va_start(ap,a)  __builtin_va_start(ap)
#define va_arg(ap,ty) __builtin_va_arg(ap,ty)
#define va_end(ap)      __builtin_va_end(ap)

#endif

