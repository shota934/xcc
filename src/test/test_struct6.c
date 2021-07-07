

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
  sam.b = 30;
  p = &sam;
  
  return p->sam1.b + 1 + sam.b;
}


