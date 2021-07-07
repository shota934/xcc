
int func(int *ptr){
  return *ptr;
}

int main(){

  int a;
  a = 10;

  return func(&a);
}
