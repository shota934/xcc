
struct sample {
  int a;
  int b;
};

int func(struct sample sam){
  return sam.a + sam.b;
}


int main(){

  struct sample sam;

  sam.a = 2;
  sam.b = 4;

  return func(sam);
}
