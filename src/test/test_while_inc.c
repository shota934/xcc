
int func(){

  int i = 0;
  int sum = 0;
  
  while(i < 10){
    sum += i;
    i++;
  }
  
  return sum;
}

int main(){
  return func();
}
