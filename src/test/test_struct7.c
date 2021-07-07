
struct sample1{
  int a;
  int b;
};


struct sample{
  int a;
  int b;
  struct sample1 sam1;
};

int main(){

  struct sample sam;
  struct sample *p;

  sam.sam1.b = 20;
  
  p = &sam;
  sam.a = 40;
  
  return sam.a + p->sam1.b + 30;
}


