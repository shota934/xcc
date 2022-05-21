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
bool_t is_float(list_t *lst);
bool_t is_pointer(list_t *lst);
bool_t is_address(list_t *lst);
bool_t is_function_pointer(list_t *lst);
bool_t is_func(object_t *obj);
bool_t is_symbol(object_t *obj);
bool_t is_array(list_t *lst);
bool_t is_integer_type(env_t *env,list_t *lst);
int calc_align(int n,int m);
bool_t is_extern(list_t *lst);
int convert_hex_to_int(char *hex);
bool_t is_var_args(list_t *lst);
bool_t is_qualifier(list_t *lst);
bool_t is_static(list_t *lst);
list_t *get_func_name(list_t *lst);
symbol_t *lookup_member(env_t *env,string_t name);
type_t conv_type(env_t *env,env_t *cenv,list_t *type_lst,list_t *lst);
int calc_array_size(list_t *lst);
bool_t is_integertype(list_t *lst);
bool_t is_compound_type(list_t *lst);
bool_t is_global_var(list_t *lst);
compound_def_t *get_comp_obj(env_t *env,env_t *cenv,string_t name);
string_t concat_strs(string_t s1,string_t s2);
int get_obj_size(object_t *obj);

#endif
