#define T2

#if defined(T1)
#define A 10
#elif defined(T2)
#define A 20
#else
#define A 30
#endif


int main(){
  return A + 30;
}
