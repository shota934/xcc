
int func(int a,int b,){
  return a + b;
}

int main(){

  int (*f)(int,int);
  int a;

  f = &func;
  a = f(1,2);
  
  return a;
}
