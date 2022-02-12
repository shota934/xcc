#include <stdio.h>
#include <stdarg.h>

int func(int num,...){

  va_list ap;

  va_start(ap,num);
  
  int *p = va_arg(ap,int*);
  printf("%d\n",*p);
  
  va_end(ap);

  return *p;
}

int main(){

  int a;
  int b;

  a = 10;
  
  b = func(1,&a);
  if(b == a){
	return 0;
  } else {
	return 1;
  }
}
