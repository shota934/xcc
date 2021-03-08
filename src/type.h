// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__TYPE__
#define __INCLUDE__TYPE__

typedef struct list_t list_t;
typedef enum list_type_t list_type_t;
typedef enum opt_type_t opt_type_t;
typedef enum object_type_t object_type_t;
typedef struct env_t env_t;
typedef struct file_t file_t;
typedef struct token_t token_t;
typedef enum token_type_t token_type_t;
typedef char * string_t;
typedef unsigned char bool_t;
typedef int integer_t;
typedef struct macro_t macro_t;
typedef struct lexer_t lexer_t;
typedef struct compound_def_t compound_def_t;
typedef struct symbol_t symbol_t;
typedef struct struct_env_t struct_env_t;
typedef struct opt_info_t opt_info_t;
typedef struct enum_env_t enum_env_t;
typedef enum type_t type_t;
typedef enum scope_t scope_t;
typedef enum kind_t kind_t;
typedef enum access_type access_type;
typedef enum macro_type_t macro_type_t;
typedef struct struct_obj_t struct_obj_t;
typedef struct stack_ty stack_ty;
typedef list_t enum_obj_t;
typedef struct compile_info_t compile_info_t;
typedef struct source_info_t source_info_t;
typedef enum decl_type_t decl_type_t;
typedef struct set_t set_t;
typedef enum compound_type_t compound_type_t;
typedef struct map_t map_t;
typedef enum type_type_t type_type_t;
typedef struct func_t func_t;
typedef struct object_t object_t;

enum kind_t {
  KIND_UNDEFINE,
  KIND_VARIABLE,
  //KIND_FUNCTION,
  KIND_ADDRESS,
  KIND_DEREF,
  KIND_ENUM,
  KIND_REF_MEMBER,
  KIND_POINTER,
  KIND_TYPE
};

enum type_t {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_SHORT,
  TYPE_LONG,
  TYPE_DOUBLE,
  TYPE_FLOAT,
  TYPE_STRUCT,
  TYPE_ENUM,
  //TYPE_FUNCTION,
  TYPE_UNDEFINE,
  TYPE_TYPE,
  TYPE_POINTER,
  TYPE_UNION
};

enum access_type {
  EXTERNAL,
  INTERNAL,
  REGISTER,
  TYPEDEF
};

enum scope_t {
  UNDEFINED,
  LOCAL,
  GLOBAL
};

enum decl_type_t {
  VARIABLE_DECL,
  FUNCTION_DECL,
  TYPEDEF_DECL
};

enum type_type_t {
  TYPE_FUNCTION,
  TYPE_SYMBOL,
  TYPE_COMPOUND
};

enum compound_type_t {
  STRUCT_COMPOUND_TYPE,
  UNION_COMPOUND_TYPE,
  OTHER_COMPOUND_TYPE
};


#endif
