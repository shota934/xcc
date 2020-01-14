#define __TEST__

int func(int a);
int func(int a){
  return a;
}

int main(){
  
  int a;
  
#ifdef __TEST__
  a = 200;
#else
  a = 100;
#endif
  
  return func(a);
}
