

int main(){

  int a;
  int *p;
  int **pp;

  a = 10;
  p = &a;
  *p = 30;
  pp = &p;
  
  return **pp;
}
  
