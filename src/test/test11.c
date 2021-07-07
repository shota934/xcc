
int func1(int a){
  return a + 30;
}

int func(int a){
  return a + func1(50);
}
  

int main(){

  int a;

  a = func(10);

  
  return a;
}
