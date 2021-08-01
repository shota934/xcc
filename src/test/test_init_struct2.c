
struct sample1 {
  int c;
  int d;
};


struct sample {
  int a;
  int b;
  struct sample1 sam;
};

int main(){

  struct sample sam = {1,2,{3,4}};
  int a;

  a = sam.a + sam.b + sam.sam.c + sam.sam.d;
  if(a == 10){
  } else {
	return 1;
  }

  return 0;
}
