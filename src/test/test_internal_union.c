
#define EXPECTED 2

struct sample{
  int a;
  union {
	int b;
	void *p;
  };
};

int main(){

  struct sample sam;
  sam.b = 2;

  if(sam.b == EXPECTED){
	return 0;
  } else {
	return 1;
  }
}
