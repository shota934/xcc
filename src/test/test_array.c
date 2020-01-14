
int func(){
  
  int array[5];
  int i = 0;

  array[i] = 40;
  i = 1;
  array[i] = 80;
  i = 4;
  array[i] = 100;
  
  return array[i];
}

int main(){
  return func();
}
