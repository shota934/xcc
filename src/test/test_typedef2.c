
typedef int * int_ptr_t;

int main(){

  int a = 20;
  int_ptr_t p = &a;
  
  return *p;
}

