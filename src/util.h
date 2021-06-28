// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__UTIL__
#define __INCLUDE__UTIL__
#include <string.h>
#include "type.h"
#include "symbol.h"

#define TABLE_SIZE 256

int hash(char *str);
bool_t is_inttype(list_t *lst);
bool_t is_decimaltype(type_t type);
type_t util_get_type(list_t *lst);
bool_t is_float(list_t  *lst);
bool_t is_struct(symbol_t *sym);
bool_t is_structtype(list_t *lst);
bool_t is_compund_type(list_t *lst);
bool_t is_pointer(list_t *lst);
bool_t is_struct_ref(list_t *lst);
bool_t is_struct_decl(list_t *lst);
bool_t is_address(list_t *lst);
bool_t is_deref(list_t *lst);
bool_t is_nedd_cast(list_t *lst);
bool_t is_function_pointer(list_t *lst);
bool_t is_func(object_t *obj);
bool_t is_symbol(object_t *obj);
bool_t is_array(list_t *lst);
bool_t is_value(list_t *lst);
bool_t is_name(list_t *lst);
kind_t select_type(list_t *lst);
int calc_align(int n,int m);
bool_t is_extern(list_t *lst);
int convert_hex_to_int(char *hex);
bool_t is_var_len_args(list_t *lst);
list_t *get_def_type_lst(list_t *lst);
bool_t is_var_args(list_t *lst);
bool_t is_compound_type(list_t *lst);
bool_t is_compound_alloc_type(list_t *lst);
bool_t is_qualifier(list_t *lst);
list_t *get_func_name(list_t *lst);
symbol_t *lookup_member(env_t *env,string_t name);
type_t conv_type(env_t *env,list_t *type_lst,list_t *lst);

#endif
