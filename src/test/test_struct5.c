
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

  sam.sam1.b = 20;
  
  return sam.sam1.b + 1;
}


