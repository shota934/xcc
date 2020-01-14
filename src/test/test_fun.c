
int func2(){
  return 10;
}

int func1(){
  return 11 + func2();
}

int func(){

  int a = 0;
  int b = 0;
  a = 20;
  b = 3 + 100;

  a = func1();

  return a;
}

int main(){

  int a;

  a = func();
  
  return a;
}
