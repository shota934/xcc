
int func(){

  int i;
  int sum;
  
  i = 0;
  sum = 0;
  while(i <= 10){
	sum += i;
	i++;
  }
  
  return sum;
}


int main(){

  int a;

  a = func();
  
  return a;
}
