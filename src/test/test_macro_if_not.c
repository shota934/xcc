
#define __TEST__

int func(int a,int b);
int func(int a,int b){

  int c = 80;

  return a + b + c;
}

int main(){
  
  int a;

  a = 20;
#ifndef __TEST__
  a = 10;
#endif
  
  return func(a,10);
}
