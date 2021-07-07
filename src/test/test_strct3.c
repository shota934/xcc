
struct sample{
  int a;
  int b;
};

int func(int a, struct sample *sam){
  return sam->a + 10 + a;
}

int main(){

  struct sample sam;
  struct sample *p;
  p = &sam;
  p->a = 40;

  return func(20,&sam);
}



