#include "enumdef.h"
#include "mem.h"

enumdef_t *enumdef_create(int val,string_t enum_class){

  enumdef_t *enumdef;

  enumdef = mem(sizeof(enumdef_t));
  enumdef->obj.type = TYPE_ENUMULATE;
  enumdef->obj.scope = ENUMLATE;
  enumdef->val = val;
  enumdef->enum_class = enum_class;

  return enumdef;
}
