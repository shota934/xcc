#include <stdio.h>

int main(){

  int a;
  int b;
  int c;
  int d;
  int ret;

  asm("movl $30,-20(%rbp)");
  asm("movl $40,-16(%rbp)");
  asm("movl $20,-12(%rbp)");

  d = a + b + c;
  if(d == 90){
	ret = 0;
  } else {
	ret = 1;
  }
  
  return ret;
}
