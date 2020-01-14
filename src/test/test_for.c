
int func(){

  int i = 0;
  int sum = 0;
  
  for(i = 0; i < 10; i = i + 1){
    sum = sum + i;
  }

  return sum;
}

int main(){
  return func();
}
