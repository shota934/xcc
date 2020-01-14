#include <stdio.h>

int func(int a,int b);
int func(int a,int b){

  return a + b + 30;
}

int main(){

  int a;
  int b;

  a = 10;
  b = 20;

  return func(a,b);
}
