
int func(int *ptr){
  return *ptr + 20;
}

int main(){

  int a;
  a = 10;

  return func(&a);
}
