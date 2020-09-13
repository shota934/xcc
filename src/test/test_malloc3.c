struct list_t {
  int num;
  struct list_t *next;
};

struct list_t *make();

struct list_t *make(int num){
  
  struct list_t *p;

  p = malloc(sizeof(struct list_t));
  p->num = num;
  
  return p;
}

int main(){

  struct list_t *q;

  q = make(140);
  
  return q->num;
}



