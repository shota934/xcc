int sum(int n){

  if(n <= 0){
    return 0;
  } else {
    return n + sum(n - 1);
  }
  
}

int main(){

  return sum(10);
  
  return 0;
}
