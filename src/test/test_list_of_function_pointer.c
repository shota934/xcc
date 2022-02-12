
int func1(){
  return 30;
}

int func2(){
  return 40;
}

int func3(int a,int b){
  
  int (*func[1][2])();
  int i;

  func[0][0] = &func1;
  func[0][1] = &func2;

  i = func[0][0]() + func[0][1]() + a + b;

  return i;
}

int main(){

  int a;
  int b;

  b = 30;
  a = func3(20,b);
  if(a == 120){
	return 0;
  } else {
	return 1;
  }
}
