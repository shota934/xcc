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
} va_list_type;

typedef va_list_type va_list[1];

#define va_start(ap,l)  
#define va_arg(ap,type)
#define va_end(ap)  1
#define va_copy(dest,src) ((dest)[0] = (src)[0])
#define __GNUC_VA_LIST 1
typedef va_list __gnuc_va_list; 


#endif

