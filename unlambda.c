#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define false 0
#define true 1


#define MAXOBJECTS 1000


typedef struct TObject Object;

typedef Object*(*Func)(Object* self, Object* other);

struct TObject {
  Object* fPrev;
  int fGeneration;
  Func fProc;
  Object * fObject1;
  Object * fObject2;
  char fChar;
};


Object* call(Object* self, Object* other){
 
  return self->fProc(self, other);
}

Object** gobjects;
int ggeneration;


void start_mem(){
  int i;

  gobjects = (Object**)malloc(sizeof(Object*)*MAXOBJECTS);
  for(i=0;i<MAXOBJECTS; i++){
    *(gobjects+i) = NULL;
  }
  ggeneration = 0;
}

void stop_mem(){
  free((void*)gobjects);
}

void make_new_entry(Object* obj){
  int i;
  for(i=0;i<MAXOBJECTS; i++){
    if( *(gobjects+i)==NULL){
      *(gobjects+i) = obj;
      return;
    }
  }
  assert(false);
}


Object* NewObject(){
  Object* r;
  r = (Object*)malloc(sizeof(Object));
  make_new_entry(r);
  r->fPrev = NULL;
  r->fObject1 = NULL;
  r->fObject2 = NULL;
  r->fGeneration = -1;
  return r;
}

void DeleteObject(Object* self){
  free((void*)self);
}


Object* gstack;

void init_stack(){
  gstack = NULL;
}

void push(Object* obj){
  obj->fPrev = gstack;
  gstack = obj;
}

Object* pop(void){
  Object* r;
  r = gstack;
  gstack = gstack->fPrev;
  return r;
}

void mark_tree(Object* obj){

  if(obj->fObject1){
    mark_tree(obj->fObject1);
  }
  if(obj->fObject2){
    mark_tree(obj->fObject2);
  }

  obj->fGeneration = ggeneration;
  printf("marking %p %d\n", obj, obj->fGeneration);
}

void mark(void){
  Object* obj;

  ggeneration += 1;
  obj = gstack;
  while(obj){
    mark_tree(obj);
    obj = obj->fPrev;
  }
}

void sweep(void){
  int i;
  Object* obj;

  printf("generation %d\n", ggeneration);
  for(i=0; i<MAXOBJECTS; i++){
    obj = *(gobjects+i);
    if(obj){
      printf("checking %p %d\n", obj, obj->fGeneration);
      if(ggeneration != obj->fGeneration){
        printf("deleting %p\n", obj);
        DeleteObject(obj);
        obj = NULL;
      }
    }
  }
}


int runnable(void){
  Object *operand, *operator, *q;
 
  operand = gstack;
  if(!operand){
    return false;
  }
  if(!operand->fProc){
    return false;
  }
  operator = operand->fPrev;

  if(!operator){
    return false;
  }
  if(!operator->fProc){
    return false;
  }
  q = operator->fPrev;
  if(!q){
    return false;
  }
  if(q->fProc){
    return false;
  }
  return true;
}



Object* quote(void){
  Object* r;
  r = NewObject();
  r->fProc = NULL;
  r->fObject1 = NULL;
  return r;
}


Object* _print(Object* self, Object* other){
  printf("%c", self->fChar);
  return other;
}

Object* print(char x){
  Object* r;
  r = NewObject();
  r->fProc = &_print;
  r->fChar = x;
  return r;
}


Object* _identity(Object* self, Object* other){
  return other;
}

Object* identity(void){
  Object* r;
  r = NewObject();
  r->fProc = &_identity;
  r->fObject1 = NULL;
  return r;
}


Object* _constant_function(Object* self, Object* other){
  return other;
}

Object* constant_function(){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = NewObject();
  r->fProc = &_constant_function;
  return r;
}




Object* _s2(Object* self, Object* other){
  /* self == s2, other == z */
  Object* x; Object* y; Object*z;
  Object* xz; Object* yz;
  Object* r;
  z = other;
  y = self->fObject1;
  x = self->fObject2->fObject1;
  xz = call(x, z);
  yz = call(y, z);
  r = call(xz, yz);

  return r;
}


Object* _s1(Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = NewObject();
  s2->fProc = &_s2;
  s2->fObject1 = other;
  s2->fObject2 = self; /* may be it is better idea to reference x, instead of s1 */
  return s2;
}


Object* _generalized_evaluation(Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = NewObject();
  s1->fProc = &_s1;
  s1->fObject1 = other;
  return s1;
}

Object* generalized_evaluation(void){
  /*
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = NewObject();
  r->fProc = &_generalized_evaluation;
  r->fObject1 = NULL;
  return r;
}


void run_once(void){
  Object *operand, *operator, *q, *r;

  operand = pop();
  operator = pop();
  q = pop();
  r = call(operator, operand);
  push(r);
}


int eval(char* xs){
  char x;

  init_stack();
  while (*xs){
    x = *xs;

    switch(x){
      case '`':
        push(quote());
        break;

      case '.':
        xs++;
        push((print(*xs)));
        break;

      case 'r':
        push((print('\n')));
        break;

      case 'i':
        push(identity());
        break;

      case 'k':
        push(constant_function());
        break;

      case 's':
        push(generalized_evaluation());
        break;

      default:
        break;
    }
    while (runnable()){
      run_once();
      mark();
      sweep();
    }
    xs++;
  }
  while (runnable()){
    run_once();
    mark();
    sweep();
  }
  gstack = NULL;
  mark();
  sweep();

  return 0;
}

int main(int args, char** argv){
  char* hw = "`r```````````.H.e.l.l.o. .w.o.r.l.di";
  char* fib =  "```s``s``sii`ki"
"`k.*``s``s`ks"
"``s`k`s`ks``s``s`ks``s`k`s`kr``s`k`sikk"
"`k``s`ksk";
  /*
   * http://www.madore.org/~david/programs/unlambda/
   */

  start_mem();

  eval(hw);
  //eval(fib);
  //printf("%s\n", hw);
  stop_mem();
  return 0;
}

