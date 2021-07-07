int main(){

  int a;
  int b;
  
  a = sizeof(int);
  b = sizeof(int *);

  if(a + b == 8){
	return 1;
  } else {
	return 0;
  }
}
