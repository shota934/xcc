// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h> 
#include <stdlib.h>
#include "parser.h"
#include "mem.h"
#include "typecheck.h"
#include "error.h"
//----
#include "dump.h"
//----

#define ARRAY         "[]"
#define FUNC_CALL     "func-call"
#define FUNC_DEF      "func-def"
#define FUNC_DECL     "func-decl"
#define STRUCT_DEF    "struc-def"
#define STRUCT_ALLOC  "struc-alloc"
#define STRUCT_TYPE   "struct"
#define NO_NAME       "no-name"
#define UNION_TYPE    "union"
#define UNION_DEF     "union-def"
#define UNION_ALLOC   "union_alloc"
#define ENUM_TYPE     "enum"
#define DECL_VAR      "decl-var"
#define TYPE_DEF      "typedef"
#define CAST          "cast"
#define TYPE          "type"
#define EXPR          "expr"

#define IS_SWITCH(t)   (TOKEN_GET_TYPE(t) == TOKEN_SWITCH)
#define IS_CASE(t)     (TOKEN_GET_TYPE(t) == TOKEN_CASE)
#define IS_DEFAULT(t)  (TOKEN_GET_TYPE(t) == TOKEN_DEFAULT)
#define IS_NUMBER(t)   (TOKEN_GET_TYPE(t) == TOKEN_NUMBER)
#define IS_FLOAT(t)    (TOKEN_GET_TYPE(t) == TOKEN_FLOAT) \
                       || (TOKEN_GET_TYPE(t) == TOKEN_DOUBLE)
#define IS_HEX(t)      (TOKEN_GET_TYPE(t) == TOKEN_HEX)
#define IS_LETTER(t)   (TOKEN_GET_TYPE(t) == TOKEN_LETTER)
#define IS_ASSIGN(t)   (TOKEN_GET_TYPE(t) == TOKEN_ASSIGN)
#define IS_LPAREN(t)   (TOKEN_GET_TYPE(t) == TOKEN_LPAREN)
#define IS_RPAREN(t)   (TOKEN_GET_TYPE(t) == TOKEN_RPAREN)
#define IS_LBRACE(t)   (TOKEN_GET_TYPE(t) == TOKEN_LBRACE)
#define IS_RBRACE(t)   (TOKEN_GET_TYPE(t) == TOKEN_RBRACE)
#define IS_EQUAL(t)    (TOKEN_GET_TYPE(t) == TOKEN_EQUAL)
#define IS_SEMI_COLON(t) (TOKEN_GET_TYPE(t) == TOKEN_SEMI_COLON)
#define IS_COMMA(t)    (TOKEN_GET_TYPE(t) == TOKEN_COMMA)
#define IS_LBRACKET(t) (TOKEN_GET_TYPE(t) == TOKEN_LBRACKET)
#define IS_RBRACKET(t) (TOKEN_GET_TYPE(t) == TOKEN_RBRACKET)
#define IS_RET(t)      (TOKEN_GET_TYPE(t) == TOKEN_RET)
#define IS_IF(t)      (TOKEN_GET_TYPE(t) == TOKEN_IF)
#define IS_ELSE(t)    (TOKEN_GET_TYPE(t) == TOKEN_ELSE)
#define IS_WHILE(t)    (TOKEN_GET_TYPE(t) == TOKEN_WHILE)
#define IS_STRING(t)  (TOKEN_GET_TYPE(t) == TOKEN_STRING)
#define IS_FOR(t)    (TOKEN_GET_TYPE(t) == TOKEN_FOR)
#define IS_ASTERISK(t) (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_MUL(t)  (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_DIV(t)  (TOKEN_GET_TYPE(t) == TOKEN_DIV)
#define IS_IF(t)   (TOKEN_GET_TYPE(t) == TOKEN_IF)
#define IS_AND(t)  (TOKEN_GET_TYPE(t) == TOKEN_AND)
#define IS_OR(t)   (TOKEN_GET_TYPE(t) == TOKEN_OR)
#define IS_XOR(t)  (TOKEN_GET_TYPE(t) == TOKEN_XOR)
#define IS_NOT(t)  (TOKEN_GET_TYPE(t) == TOKEN_NOT)
#define IS_TILDE(t) (TOKEN_GET_TYPE(t) == TOKEN_TILDE)
#define IS_AND_BOOL(t) (TOKEN_GET_TYPE(t) == TOKEN_BOOL_AND)
#define IS_OR_BOOL(t)  (TOKEN_GET_TYPE(t) == TOKEN_BOOL_OR)
#define IS_POINTER(t) (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_TYPE(t)     (TOKEN_GET_TYPE(t) == TOKEN_INT)         \
                       || (TOKEN_GET_TYPE(t) == TOKEN_VOID)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_CHAR)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_FLOAT)    \
                       || (TOKEN_GET_TYPE(t) == TOKEN_SHORT)    \
                       || (TOKEN_GET_TYPE(t) == TOKEN_LONG)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_DOUBLE)   \
                       || (TOKEN_GET_TYPE(t) == TOKEN_SIGNED)   \
                       || (TOKEN_GET_TYPE(t) == TOKEN_UNSIGNED) \

#define IS_END_T(t)    (TOKEN_GET_TYPE(t) == TOKEN_EOT)
#define IS_COP(t)      ((TOKEN_GET_TYPE(t) == TOKEN_LESS) || \
			(TOKEN_GET_TYPE(t) == TOKEN_GREATER) || \
			(TOKEN_GET_TYPE(t) == TOKEN_LESS_EQUAL) || \
			(TOKEN_GET_TYPE(t) == TOKEN_GREATER_EQUAL) || \
			(TOKEN_GET_TYPE(t) == TOKEN_EQUAL)         || \
                        (TOKEN_GET_TYPE(t) == TOKEN_NOT_EQUAL))

#define IS_CREMENT_OPERATOR(t) ((TOKEN_GET_TYPE(t) == TOKEN_INCREMENT_BY_ONE) || \
				(TOKEN_GET_TYPE(t) == TOKEN_DECREMENT_BY_ONE))
#define IS_CREMENT_ASSING_OPERATOR(t)  ((TOKEN_GET_TYPE(t) == TOKEN_INCREMENT) || \
					(TOKEN_GET_TYPE(t) == TOKEN_DECREMENT))
#define IS_SIZEOF(t)      (TOKEN_GET_TYPE(t) == TOKEN_SIZE_OF)
#define IS_STRUCT(t)      (TOKEN_GET_TYPE(t) == TOKEN_STRUCT)
#define IS_TYPEDEF(t)     (TOKEN_GET_TYPE(t) == TOKEN_TYPEDEF)
#define IS_DOT(t)         (TOKEN_GET_TYPE(t) == TOKEN_DOT)
#define IS_BREAK(t)       (TOKEN_GET_TYPE(t) == TOKEN_BREAK) 
#define IS_COLON(t)       (TOKEN_GET_TYPE(t) == TOKEN_COLON)
#define IS_ARROW_OP(t)    (TOKEN_GET_TYPE(t) == TOKEN_ARROW_OP)
#define IS_ENUM(t)        (TOKEN_GET_TYPE(t) == TOKEN_ENUM)
#define IS_STATIC(t)      (TOKEN_GET_TYPE(t) == TOKEN_STATIC)
#define IS_EXTERN(t)      (TOKEN_GET_TYPE(t) == TOKEN_EXTERN)
#define IS_CONST(t)       (TOKEN_GET_TYPE(t) == TOKEN_CONST)
#define IS_REGISTER(t)    (TOKEN_GET_TYPE(t) == TOKEN_REGISTER)
#define IS_AUTO(t)        (TOKEN_GET_TYPE(t) == TOKEN_AUTO)
#define IS_UNION(t)       (TOKEN_GET_TYPE(t) == TOKEN_UNION)
#define IS_VAR_ARG_LIST(t) (TOKEN_GET_TYPE(t) ==  TOKEN_ARGMENT_LIST)
#define IS_FUNC_DECL(l)   (length_of_list(l) == 3) && (IS_LIST(l))
#define IS_STRUCT_OR_UNION_DEF(l)  (length_of_list(l) == 2)

#define IS_DEFINED(t)    (TOKEN_GET_TYPE(t) == TOKEN_DEFINED)
#define IS_BIT_SHIFT(t)  ((TOKEN_GET_TYPE(t) == TOKEN_BIT_LEFT_SHIFT) || (TOKEN_GET_TYPE(t) == (TOKEN_GET_TYPE(t) == TOKEN_BIT_LEFT_SHIFT)))

static list_t *parser_parse_def(parser_t *parser);
static list_t *parser_parse_storage_class_dcl(parser_t *parser);

static list_t *parser_parse_type(parser_t *parser,int flag_of_typedef);
static list_t *parser_parse_ext_dcls(parser_t *parser);
static list_t *parser_parse_dclation(parser_t *parser);
static list_t *parser_parse_declvar(list_t *type,list_t *lst,list_t *init_lst);
static list_t *parser_parse_func(parser_t *parser,list_t *type,list_t *lst,list_t *q,decl_type_t decl_type);
static list_t *parser_parse_enum(parser_t *parser);
static list_t *parser_parse_enum_elements(parser_t *parser);
static list_t *parser_parse_enum_element(parser_t *parser);
static list_t *parser_parse_typedef_union_or_struct(parser_t *parser,list_t *lst,bool_t struct_type);
static list_t *parser_parse_union_or_struct(parser_t *parser);
static list_t *parser_parse_def_struct(parser_t *parser);
static list_t *parser_parse_members(parser_t *parser);
static list_t *parser_parse_member(parser_t *parser);
static list_t *parser_parse_funcargs(parser_t *parser,int cnt);
static list_t *parser_parse_funcarg(parser_t *parser);
static list_t *parser_parse_defvars(parser_t *parser);

static list_t *parser_constant_text(parser_t *parser);
static list_t *parser_parse_defined(parser_t *parser);

static list_t *parser_parse_stmts(parser_t *parser);
static list_t *parser_parse_stmt(parser_t *parser);
static list_t *parser_parse_array(parser_t *parser);
static list_t *parser_parse_stmt_expr(parser_t *parser);
static list_t *parser_parse_vars(parser_t *parser);
static list_t *parser_parse_var(parser_t *parser);
static list_t *parser_parse_while(parser_t *parser);
static list_t *parser_parse_switch(parser_t *parser);
static list_t *parser_parse_switch_cases(parser_t *parser);
static list_t *parser_parse_switch_case(parser_t *parser);
static list_t *parser_parse_for(parser_t *parser);
static list_t *parser_parse_cond_expr(parser_t *parser);
static list_t *parser_parse_lexpr(parser_t *parser);
static list_t *parser_parse_expr(parser_t *parser);
static list_t *parser_parse_cexpr(parser_t *parser);
static list_t *parser_parse_bexpr(parser_t *parser);
static list_t *parser_parse_term(parser_t *parser);
static list_t *parser_parse_factor(parser_t *parser);
static list_t *parser_parse_if(parser_t *parser);
static list_t *parser_baracket_expr(parser_t *parser);
static list_t *parser_parse_else(parser_t *parser);
static list_t *parser_parse_func_call(parser_t *parser);
static list_t *parser_parse_func_args(parser_t *parser);
static list_t *parser_parse_func_arg(parser_t *parser);
static list_t *parser_parse_array_index(parser_t *parser);
static list_t *parser_parse_sizeoftype(parser_t *parser);
static void list_set_symbol_type(list_t *lst,type_t type,kind_t kind);

static list_t *parser_parse_dcl(parser_t *parser);

static list_t *parser_parse_dirdcl(parser_t *parser);
static list_t *parser_parse_dirdclvar(parser_t *parser,list_t *var);
static list_t *parser_parse_initializer(parser_t *parser);
static list_t *parser_parse_block_items(parser_t *parser);
static list_t *parser_parse_block_item(parser_t *parser);
static list_t *parser_parse_type_qualifier_list(parser_t *parser);
static int is_type_qualifier(token_t *t);

static int is_dclvar(list_t *lst);
static int is_dcl(list_t *lst);
static int is_typedef_name(parser_t *parser,token_t *token);

parser_t *parser_create(){
  
  parser_t *parser = mem(sizeof(parser_t));
  parser_init(parser);
  
  return parser;
}

void parser_init(parser_t *parser){

  parser->var_lst = make_null();
  parser->set = set_create();
  
  return;
}

list_t *parser_parse(parser_t *parser){

  env_t *env;
  
  env = make_env();
  return parser_parse_def(parser);
}

void parser_set_src(parser_t *parser,char *src){
  
  lexer_set_src(PARSER_GET_LEX(parser),src);
  
  return;
}

list_t *parser_constant_texts(parser_t *parser){

  list_t *expr;
  
  #ifdef __DEBUG__
  printf("parser_constant_texts\n");
  #endif
  
  expr = parser_constant_text(parser);
  
  return expr;
}

static list_t *parser_constant_text(parser_t *parser){

  list_t *expr;
  
  #ifdef __DEBUG__
  printf("parser_constant_text\n");
  #endif

  expr = parser_parse_cond_expr(parser);
  
  return expr;
}

static list_t *parser_parse_defined(parser_t *parser){

  list_t *expr;
  token_t *t;
  int flag;
  

  #ifdef __DEBUG__
  printf("parser_parse_defined\n");
  #endif

  flag = FALSE;
  t = lexer_get_token(PARSER_GET_LEX(parser));
  expr = make_null();
  if(IS_LPAREN(t)){
    flag = TRUE;
  } else if(IS_LETTER(t)){
    expr = add_symbol(expr,TOKEN_GET_STR(t));
  }

  if(flag){
    t = lexer_get_token(PARSER_GET_LEX(parser));
    expr = add_symbol(expr,TOKEN_GET_STR(t));
    
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
      
    }
  }
  
  return expr;
}

static list_t *parser_parse_def(parser_t *parser){ 

  list_t *cpp_lst;
  list_t *new_lst;
  
  new_lst = parser_parse_ext_dcls(parser);

  return new_lst;
}

static list_t *parser_parse_enum(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  t = lexer_get_token(PARSER_GET_LEX(parser));
  
  new_lst = add_symbol(make_null(),TOKEN_GET_STR(t));
  list_set_symbol_type(new_lst,TYPE_ENUM,KIND_ENUM);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    list_t *elements = parser_parse_enum_elements(parser);
    new_lst = concat(new_lst,add_list(make_null(),elements));
  }
  
  return new_lst;
}

static list_t *parser_parse_enum_elements(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  new_lst = parser_parse_enum_element(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    list_t *element_lst = parser_parse_enum_elements(parser);
    new_lst = concat(new_lst,element_lst);
  } else if(IS_RBRACE(t)){
    return new_lst;
  }
  
  return new_lst;
}

static list_t *parser_parse_enum_element(parser_t *parser){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_enum_element\n");
#endif

  new_lst = parser_parse_cexpr(parser);
  
  return new_lst;
}

static list_t *parser_parse_typedef_union_or_struct(parser_t *parser,list_t *lst,bool_t struct_type){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_typedef_union_or_struct\n");
#endif
  
  if(struct_type){
    if(IS_LIST(cdr(lst))){
      new_lst = add_symbol(lst,STRUCT_DEF);
    } else {
      new_lst = add_symbol(lst,STRUCT_TYPE);
    }
  } else {
    if(IS_LIST(cdr(lst))){
      new_lst = add_symbol(lst,UNION_DEF);
    } else {
      new_lst = add_symbol(lst,UNION_TYPE);
    }
  }
  return new_lst;
}

static list_t *parser_parse_union_or_struct(parser_t *parser){
  
  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_union_or_struct\n");
#endif
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LETTER(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    t = lexer_get_token(PARSER_GET_LEX(parser));
  } else {
    new_lst = add_symbol(new_lst,NO_NAME);
  }
  
  if(IS_LBRACE(t)){
    new_lst = concat(new_lst,add_list(make_null(),parser_parse_members(parser)));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_members(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_members\n");
#endif
  
  new_lst = parser_parse_member(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));  
  if(IS_RBRACE(t)){
    return new_lst;
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    return concat(new_lst,parser_parse_members(parser));
  }
}

static list_t *parser_parse_member(parser_t *parser){
  
  list_t *new_lst;
  list_t *type_lst;
  list_t *quali_type_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_member\n");
#endif
  
  new_lst = make_null();
  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,FALSE);
  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst,VARIABLE_DECL);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_SEMI_COLON(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",";",TOKEN_GET_STR(t));
  }
  
  return add_list(make_null(),new_lst);
}

static list_t *parser_parse_storage_class_dcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_storage_class_dcl\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_null();
  if(IS_STATIC(t)){
    new_lst = add_number(new_lst,INTERNAL);
  } else if(IS_EXTERN(t)){
    new_lst = add_number(new_lst,EXTERNAL);
  } else if(IS_REGISTER(t)){
    new_lst = add_number(new_lst,REGISTER);
  } else if(IS_TYPEDEF(t)){
    new_lst = add_number(new_lst,TYPEDEF);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_type(parser_t *parser,int flag_of_typedef){

  list_t *new_lst;
  list_t *qu_ty_lst;
  token_t *t;

  new_lst = make_null();
#ifdef __DEBUG__
  printf("parser_parse_type\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_TYPE(t) || is_typedef_name(parser,t)){
    new_lst = parser_parse_type(parser,flag_of_typedef);
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
  } else if(IS_STRUCT(t) || IS_UNION(t)){
    bool_t flag = TRUE;
    new_lst = parser_parse_union_or_struct(parser);
    if(flag_of_typedef){
      new_lst = parser_parse_typedef_union_or_struct(parser,new_lst,flag);
    } else {
      if(IS_LIST(cdr(new_lst))){
	new_lst = add_symbol(new_lst,STRUCT_DEF);
      } else {
	new_lst = add_symbol(new_lst,STRUCT_ALLOC);
      }
    }
  } else if (IS_UNION(t)){
      if(flag_of_typedef){
	new_lst = add_symbol(new_lst,UNION_DEF);
      } else {
	if(IS_LIST(cdr(new_lst))){
	  new_lst = add_symbol(new_lst,UNION_DEF);
	} else {
	  new_lst = add_symbol(new_lst,UNION_ALLOC);
	}
      }
  } else if(IS_ENUM(t)){
    if(flag_of_typedef){
      t = lexer_get_token(PARSER_GET_LEX(parser));
      new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
      new_lst = add_symbol(new_lst,ENUM_TYPE);
    }
  } else if(IS_LETTER(t) && set_find_obj(PARSER_GET_SET(parser),TOKEN_GET_STR(t))){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_ext_dcls(parser_t *parser){
  
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_ext_dcls\n");
#endif
  new_lst = parser_parse_dclation(parser);
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(new_lst,parser_parse_ext_dcls(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_dclation(parser_t *parser){

  list_t *new_lst;
  list_t *sub_lst;
  list_t *type_lst;
  list_t *dcl_lst;
  list_t *quali_type_lst;
  token_t *t;
  type_t type;
  decl_type_t decl_type;
  int flag;
  list_t *q;
  
#ifdef __DEBUG__
  printf("parser_parse_dclation\n");
#endif
  type = TYPE_UNDEFINE;
  dcl_lst = parser_parse_storage_class_dcl(parser);
  if(TYPEDEF == *(int *)(car(dcl_lst))){
    decl_type = TYPEDEF_DECL;
    flag = TRUE;
  } else {
    decl_type = FUNCTION_DECL;
    flag = FALSE;
  }
  
  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,flag);
  if(IS_NULL_LIST(type_lst)){
    return dcl_lst;
  }
  
  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst,decl_type);
  new_lst = concat(dcl_lst,new_lst);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    new_lst = concat(new_lst,add_list(make_null(),parser_parse_block_items(parser)));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_RBRACE(t)){
      new_lst = concat(add_symbol(make_null(),FUNC_DEF),new_lst);
      new_lst = concat(new_lst,add_list(make_null(),PARSER_GET_VAR_LST(parser)));
      PARSER_SET_VAR_LST(parser,make_null());
      new_lst = add_list(make_null(),new_lst);
    }
  } else if(IS_ASSIGN(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = concat(new_lst,parser_parse_initializer(parser));
    new_lst = add_list(make_null(),new_lst);
    t = lexer_get_token(PARSER_GET_LEX(parser));    
    if(!IS_SEMI_COLON(t)){
      lexer_put_token(PARSER_GET_LEX(parser),t);
    }
  } else if(IS_SEMI_COLON(t)){
    if(IS_FUNC_DECL(new_lst)){
      new_lst = add_list(make_null(),concat(add_symbol(make_null(),FUNC_DECL),new_lst));
    } else if(!IS_LIST(new_lst)){
      new_lst = add_list(make_null(),new_lst);
    }
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}


static list_t *parser_parse_declvar(list_t *type,list_t *lst,list_t *init_lst){

  list_t *new_lst;

  new_lst = make_null();
  
  return new_lst;
}

static list_t *parser_parse_func(parser_t *parser,list_t *type,list_t *lst,list_t *q,decl_type_t decl_type){
  
  list_t *new_lst;
  list_t *p;
  list_t *pointer_end;

#ifdef __DEBUG__
  printf("parser_parse_func\n");
#endif

  pointer_end = make_null();
  if(IS_NULL_LIST(type) && IS_NULL_LIST(type)){
    return type;
  }
  
  pointer_end = make_null();
  for(p = lst; STRCMP(car(p),"*"); p = cdr(p)){
    pointer_end = p;
  }

  if(IS_NOT_NULL_LIST(pointer_end)){
    LIST_SET_NEXT(pointer_end,make_null());
    new_lst = reverse(concat(type,lst));
    new_lst = add_list(make_null(),new_lst);
  } else {
    if(!is_dcl(type)){
      type = reverse(type);
    }
    new_lst = add_list(make_null(),type);
  }
  
  new_lst = concat(new_lst,p);
  new_lst = concat(q,new_lst);

  if(FUNCTION_DECL == decl_type){
    if(is_dclvar(new_lst)){
      new_lst = add_list(make_null(),new_lst);
      new_lst = add_symbol(new_lst,DECL_VAR);
      PARSER_SET_VAR_LST(parser,concat(PARSER_GET_VAR_LST(parser),add_list(make_null(),new_lst)));
      new_lst = add_list(make_null(),new_lst);
    }
  } else if(VARIABLE_DECL == decl_type){
    if(is_dclvar(new_lst)){
      new_lst = add_list(make_null(),new_lst);
    }
  } else if(TYPEDEF_DECL == decl_type){
    set_add_obj(PARSER_GET_SET(parser),car(cdr(new_lst)));
  }
  
  return new_lst;
}

static list_t *parser_parse_funcargs(parser_t *parser,int cnt){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_funcargs\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_RPAREN(t)){
    new_lst = make_null();
  } else if(IS_VAR_ARG_LIST(t) && cnt == 1){
    new_lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_RPAREN(t)){
      return new_lst;
    }
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    new_lst = parser_parse_funcarg(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_COMMA(t)){
      new_lst = concat(new_lst,parser_parse_funcargs(parser,cnt + 1));
    } else if(IS_RPAREN(t)){
      return new_lst;
    }
  }

  return new_lst;
}

static list_t *parser_parse_funcarg(parser_t *parser){

  list_t *new_lst;
  list_t *type_lst;
  list_t *quali_type_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_funcarg\n");
#endif

  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,FALSE);
  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst,VARIABLE_DECL);

  return new_lst;
}

static list_t *parser_parse_block_items(parser_t *parser){
  
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_block_items\n");
#endif  
  new_lst = parser_parse_block_item(parser);
  
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(add_list(make_null(),new_lst),parser_parse_block_items(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_block_item(parser_t *parser){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_block_item\n");
#endif
  new_lst = parser_parse_dclation(parser);
  if(IS_NULL_LIST(new_lst)){
    new_lst = parser_parse_stmt(parser);
  }

  return new_lst;
}

static list_t *parser_parse_type_qualifier_list(parser_t *parser){
  
  token_t *t;
  list_t *new_lst;
  
#ifdef __DEBUG__
  printf("parser_parse_type_qualifier_list\n");
#endif
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(is_type_qualifier(t)){
    list_t *const_lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    new_lst = concat(const_lst,parser_parse_type_qualifier_list(parser));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static int is_type_qualifier(token_t *t){
  
  int flag;
  
  flag = FALSE;
  if(IS_CONST(t)){
    flag = TRUE;
  }
  
  return flag;
}

static list_t *parser_parse_stmts(parser_t *parser){

  list_t *new_lst;
  
#ifdef __DEBUG__
  printf("parser_parse_stmts\n");
#endif
  
  new_lst = parser_parse_stmt(parser);
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(new_lst,parser_parse_stmts(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_stmt(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_stmt\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_null();
  if(IS_SEMI_COLON(t)){
	new_lst = add_list(make_null(),new_lst);
	return new_lst;
  } else if(IS_RET(t)){
    new_lst = concat(new_lst,add_list(make_null(),parser_parse_cond_expr(parser)));
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_IF(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_if(parser)));
  } else if(IS_SWITCH(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_switch(parser)));
  } else if(IS_WHILE(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_while(parser)));
  } else if(IS_FOR(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_for(parser)));
  } else if(IS_BREAK(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_END_T(t)){
    new_lst = make_null();
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    new_lst = parser_parse_stmt_expr(parser);
  }
  
  if((!IS_IF(t)) && (!IS_WHILE(t)) && (!IS_FOR(t))){
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_SEMI_COLON(t)){
      lexer_put_token(PARSER_GET_LEX(parser),t);
    }
  }
  
  return new_lst;
}

static list_t *parser_parse_array(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_array\n");
#endif
  
  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = add_list(make_null(),add_symbol(new_lst,ARRAY));
  
  return new_lst;
}

static list_t *parser_parse_stmt_expr(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_stmt_expr\n");
#endif
  new_lst = make_null();
  new_lst = parser_parse_cond_expr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_ASSIGN(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = concat(new_lst,parser_parse_cond_expr(parser));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_CREMENT_ASSING_OPERATOR(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = concat(new_lst,parser_parse_cond_expr(parser));
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_vars(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_vars\n");
#endif
  new_lst = parser_parse_var(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    new_lst = concat(new_lst,parser_parse_var(parser));
  } else if(IS_SEMI_COLON(t)){
    return new_lst;
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_var(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_var\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = add_symbol(make_null(),TOKEN_GET_STR(t));
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACKET(t)){
    new_lst = concat(new_lst,parser_parse_array(parser));
    new_lst = add_list(make_null(),new_lst);
    new_lst = add_symbol(new_lst,ARRAY);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_ASSIGN(t)){
    lexer_put_token(PARSER_GET_LEX(parser),t);
  } else {
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = concat(new_lst,parser_parse_cexpr(parser));
    new_lst = add_list(make_null(),new_lst);
  }
  
  return new_lst;
}

static list_t *parser_parse_cond_expr(parser_t *parser){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_cond_expr\n");
#endif
  
  return parser_parse_lexpr(parser);
}

static list_t *parser_parse_bexpr(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_bexpr\n");
#endif

  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_AND(t) || IS_OR(t) || IS_XOR(t)){
    list_t *lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_AND_BOOL(t) || IS_OR_BOOL(t)){
    list_t *lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_BIT_SHIFT(t)){
    list_t *lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_cexpr(parser_t *parser){

  list_t *new_lst;
  list_t *lst;
  list_t *r;
  token_t *t;
  
  new_lst = parser_parse_expr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COP(t)){
    lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    r = parser_parse_cexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_lexpr(parser_t *parser){

  #ifdef __DEBUG__
  printf("parser_parse_lexpr\n");
  #endif

  list_t *new_lst;
  token_t *t;
  
  new_lst = parser_parse_bexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if((TOKEN_GET_TYPE(t) == TOKEN_BOOL_AND)
     || (TOKEN_GET_TYPE(t) == TOKEN_BOOL_OR)){
    list_t *op = add_symbol(make_null(),TOKEN_GET_STR(t));
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);    
  }
  
  return new_lst;
}

static list_t *parser_parse_expr(parser_t *parser){

  list_t *new_lst;
  list_t *op;
  list_t *r;
  token_t *t;

  new_lst = parser_parse_term(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(TOKEN_GET_TYPE(t) == TOKEN_ADD){
    op= add_symbol(make_null(),TOKEN_GET_STR(t));
    r = parser_parse_expr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(TOKEN_GET_TYPE(t) == TOKEN_SUB){
    if(IS_NULL_LIST(new_lst)){
      new_lst = add_number(make_null(),0);
    }
    op= add_symbol(make_null(),TOKEN_GET_STR(t));
    r = parser_parse_expr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_term(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  new_lst = parser_parse_factor(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_MUL(t)
     || IS_DIV(t)){
    list_t *l = add_symbol(make_null(),TOKEN_GET_STR(t));
    list_t *r = parser_parse_term(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(l,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_factor(parser_t *parser){
  
  list_t *new_lst;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_NUMBER(t)){
    new_lst = add_number(new_lst,atoi(TOKEN_GET_STR(t)));
  } else if(IS_HEX(t)){
    new_lst = add_number(new_lst,convert_hex_to_int(TOKEN_GET_STR(t)));
  } else if(IS_FLOAT(t)){
    new_lst = add_float(new_lst,atof(TOKEN_GET_STR(t)));
  } else if(IS_STRING(t)){
    new_lst = add_string(new_lst,TOKEN_GET_STR(t));
  } else if(IS_DEFINED(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = concat(new_lst,parser_parse_defined(parser));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_LETTER(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_LPAREN(t)){
      new_lst = concat(new_lst,parser_parse_func_call(parser));
      new_lst = add_symbol(new_lst,FUNC_CALL);
      new_lst = add_list(make_null(),new_lst);
    } else if(IS_LBRACKET(t)){
      new_lst = add_symbol(new_lst,ARRAY);
      new_lst = add_list(make_null(),concat(new_lst,parser_parse_array_index(parser)));
    } else if(IS_CREMENT_OPERATOR(t)){
      new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
      new_lst = add_list(make_null(),new_lst);
    } else if((IS_DOT(t)) || IS_ARROW_OP(t)){
      new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
      t = lexer_get_token(PARSER_GET_LEX(parser));
      if(IS_LETTER(t)){
	new_lst = concat(new_lst,add_symbol(make_null(),TOKEN_GET_STR(t)));
	new_lst = add_list(make_null(),new_lst);
      } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
      }
    } else {
      lexer_put_token(PARSER_GET_LEX(parser),t);
    }
  } else if(IS_LPAREN(t)){
    new_lst = parser_baracket_expr(parser);
  } else if(IS_AND(t) || IS_ASTERISK(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_LETTER(t)){
      ;
    } else {
      new_lst = add_list(make_null(),add_symbol(new_lst,TOKEN_GET_STR(t)));
    }
  } else if(IS_NOT(t) || IS_TILDE(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_stmt_expr(parser)));
  } else if(IS_SIZEOF(t)){
    new_lst = add_symbol(new_lst,TOKEN_GET_STR(t));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_LPAREN(t)){
      new_lst = concat(new_lst,parser_parse_sizeoftype(parser));
      t = lexer_get_token(PARSER_GET_LEX(parser));
      if(!IS_RPAREN(t)){
      }
    } else {
      list_t *expr = add_symbol(parser_parse_expr(parser),EXPR);
      new_lst = concat(new_lst,expr);
    }
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_baracket_expr(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  new_lst = parser_parse_type(parser,FALSE);
  if(IS_NULL_LIST(new_lst)){
    new_lst = parser_parse_bexpr(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_RPAREN(t)){
      lexer_put_token(PARSER_GET_LEX(parser),t);
    } else if(IS_END_T(t)){
      new_lst = make_null();
    }
  } else {
    new_lst = add_list(make_null(),new_lst);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
      exit(1);
    }
    new_lst = concat(new_lst,parser_parse_cond_expr(parser));
    new_lst = add_symbol(new_lst,CAST);
    new_lst = add_list(make_null(),new_lst);
  }
    
  return new_lst;
}

static list_t *parser_parse_if(parser_t *parser){
  
  list_t *new_lst;
  list_t *seq_stmts;
  list_t *alt_stmts;
  token_t *t;
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    
  }

  new_lst = parser_parse_bexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    seq_stmts = add_list(make_null(),parser_parse_block_items(parser));
    t = lexer_get_token(PARSER_GET_LEX(parser));;
    if(!IS_RBRACE(t)){
      exit(1);
    }
  } else {
    seq_stmts = add_list(make_null(),parser_parse_block_item(parser));
  }
  new_lst = concat(new_lst,seq_stmts);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_ELSE(t)){
    list_t *alt_stmts = parser_parse_else(parser);
    new_lst = concat(new_lst,alt_stmts);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_else(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    new_lst = add_list(make_null(),parser_parse_block_items(parser));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RBRACE(t)){
      exit(1);
    }
  } else {
    new_lst = add_list(make_null(),parser_parse_block_item(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_while(parser_t *parser){

  list_t *new_lst;
  list_t *cond;
  list_t *items;
  token_t *t;

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){

  }
  
  cond = parser_parse_stmt_expr(parser);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LBRACE(t)){
   
  }

  items = parser_parse_block_items(parser);

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
    
  }
  
  new_lst = concat(cond,items);
  
  return new_lst;
}

static list_t *parser_parse_switch(parser_t *parser){

  list_t *new_lst;
  list_t *sub_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_switch\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","(",TOKEN_GET_STR(t));
  }
  
  new_lst = parser_parse_cond_expr(parser);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",")",TOKEN_GET_STR(t));
    exit(1);
  }

  new_lst = concat(new_lst,parser_parse_stmt_expr(parser));

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LBRACE(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","{",TOKEN_GET_STR(t));
    exit(1);
  }

  new_lst = concat(new_lst,add_list(make_null(),parser_parse_switch_cases(parser)));
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","}",TOKEN_GET_STR(t));
    exit(1);
  }
  
  return new_lst;
}

static list_t *parser_parse_switch_cases(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_switch_cases\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if((IS_CASE(t)) || (IS_DEFAULT(t))){
    new_lst = add_symbol(make_null(),TOKEN_GET_STR(t));
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_switch_case(parser)));
    new_lst = concat(new_lst,parser_parse_switch_cases(parser));
  } else {
    new_lst = make_null();
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_switch_case(parser_t *parser){
  
  list_t *new_lst;
  list_t *stmt_lst;
  token_t *t;

  #ifdef __DEBUG__
  printf("parser_parse_switch_carse\n");
  #endif
  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_COLON(t)){
    
  }
  
  stmt_lst = add_list(make_null(),parser_parse_stmts(parser));
  new_lst = concat(new_lst,stmt_lst);
  new_lst = add_list(make_null(),new_lst);
  
  return new_lst;
}


static list_t *parser_parse_for(parser_t *parser){
  
  list_t *new_lst;
  list_t *items;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    
  }
  
  new_lst = add_list(make_null(),parser_parse_stmt_expr(parser));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_SEMI_COLON(t)){
    
  }

  new_lst = concat(new_lst,add_list(make_null(),parser_parse_stmt_expr(parser)));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_SEMI_COLON(t)){
    
  }
    
  new_lst = concat(new_lst,add_list(make_null(),parser_parse_stmt_expr(parser)));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LBRACE(t)){
    
  }

  items = add_list(make_null(),parser_parse_block_items(parser));
  new_lst = concat(new_lst,items);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
    
  }

  return new_lst;
}

static list_t *parser_parse_func_call(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_RPAREN(t)){
    new_lst = make_null();
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    new_lst = parser_parse_func_args(parser);
  }

  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *parser_parse_func_args(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  new_lst = parser_parse_func_arg(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    new_lst = concat(new_lst,parser_parse_func_args(parser));
  } else if(IS_RPAREN(t)){
    return new_lst;
  } else {

  }

  return new_lst;
}

static list_t *parser_parse_func_arg(parser_t *parser){

  list_t *new_lst;

  new_lst = parser_parse_cexpr(parser);

  return new_lst;
}

static list_t *parser_parse_array_index(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
    
  }

  return new_lst;
}

static list_t *parser_parse_sizeoftype(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  #ifdef __DEBUG__
  printf("parser_parse_sizeoftype\n");
  #endif

  new_lst = parser_parse_dclation(parser);
  new_lst = add_symbol(new_lst,TYPE);
  
  return new_lst;
}

static void list_set_symbol_type(list_t *lst,type_t type,kind_t kind){
  
  LIST_SET_OPERATOR_TYPE(lst,type);
  LIST_SET_SYMBOL_KIND(lst,kind);

  return;
}

static list_t *parser_parse_dcl(parser_t *parser){
  
  list_t *new_lst;
  token_t *t;
  
  new_lst = make_null();
  for(t = lexer_get_token(PARSER_GET_LEX(parser)); IS_ASTERISK(t);
      t = lexer_get_token(PARSER_GET_LEX(parser))){
    new_lst = concat(new_lst,add_symbol(make_null(),TOKEN_GET_STR(t)));
  }

  lexer_put_token(PARSER_GET_LEX(parser),t);
  new_lst = concat(new_lst,parser_parse_dirdcl(parser));
  
  return new_lst;
}

static list_t *parser_parse_dirdcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LETTER(t)){
    new_lst =  add_symbol(make_null(),TOKEN_GET_STR(t));
  } else if(IS_LPAREN(t)){
    new_lst = parser_parse_dcl(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
      
    }
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);    
  }
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACKET(t)){
    new_lst = concat(new_lst,parser_parse_array(parser));
    new_lst = concat(new_lst,parser_parse_dirdcl(parser));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_LPAREN(t)){
    new_lst = concat(new_lst,add_list(make_null(),parser_parse_funcargs(parser,0)));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_initializer(parser_t *parser){
  
  list_t *new_lst;
  
  new_lst = parser_parse_cexpr(parser);
  
  return new_lst;

}

static int is_dclvar(list_t *lst){
  return (2 == length_of_list(lst)) || IS_LIST(cdr(lst));
}

static int is_dcl(list_t *lst){
  return STRCMP(STRUCT_DEF,car(lst)) || STRCMP(UNION_DEF,car(lst));
}

static int is_typedef_name(parser_t *parser,token_t *token){

  set_t *set;

  set = PARSER_GET_SET(parser);
  if(set_find_obj(set,TOKEN_GET_STR(token))){
    return TRUE;
  } else {
    return FALSE;
  }
}

