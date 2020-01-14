
int func(const int , const int);
int func(const int a, const int b){
  
  const int i = 10;
  
  return a + b + i;
}

int main(){
  return func(10,20);
}


