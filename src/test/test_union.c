//#include <stdio.h>

union sample_t  {
  int a;
  void *p2;
};

int main(){

  union sample_t a;
  
  a.a = 20;
  a.a = a.a + 30;
  
  return a.a;
}
