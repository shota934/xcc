
int func(int a,int b){

  if(a < b){
	return a;
  } else {
	return b;
  }
  
  return 0;
}

int main(){

  int a;
  int b;

  a = 10;
  b = 20;
  
  return func(a,b);
}
