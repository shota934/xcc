struct list_t {
  int num;
  struct list_t *next;
};

int make(){

  struct list_t *p;
  
  p = malloc(sizeof(struct list_t));
  p->num = 10;
  
  return p->num;
}

int main(){
  return make();
}


