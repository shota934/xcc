
int func(int a);
int func1(int a,int b);
int func(int a){
  return a * 2;
}

int func1(int a,int b){
  return a + b;
}

int main(){

  int a;
  int b;

  a = 30;
  
  b = func(a);
  
  return b;
}


