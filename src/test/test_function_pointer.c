
int func();
int func(){
  return 10 + 20;
}

int main(){

  int (*f)();

  f = &func;

  return f();
}
