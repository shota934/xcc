

int func(int *a){
  return *a + 20;
}

int main(){

  int b = 20;
  int a = func(&b);
  
  return a;
}
