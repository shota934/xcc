
int func(int *p){

  *p = 10;
  
  return;
}

int main(){

  int a;

  a = 20;
  
  func(&a);
  
  return a;
}
