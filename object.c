#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object.h"

int gdebug = 0;
void print_obj(Object* obj, char* msg){

  printf("%s %s %p\n", msg, obj->fName, obj);

}

Object* call(Object* self, Object* other){
  if(gdebug){
      print_obj(self, "operator");
      print_obj(other, "operand");
  }

  return self->fProc(self, other);
}

Object** gobjects;
int ggeneration;


void start_mem(){
  int i;

  gobjects = (Object**)malloc(sizeof(Object*)*MAXOBJECTS);
  if(gdebug){
    printf("start/end: %p %p\n", gobjects, gobjects+MAXOBJECTS);
    /*
     * start/end: 0x4c2d040 0x4c2ef80
     */
  }
  for(i=0;i<MAXOBJECTS; i++){
    *(gobjects+i) = NULL;
  }
  ggeneration = 0;
}


void stop_mem(){
  free((void*)gobjects);
}

void mem_stat(){
  int i;
  int used;
  used = 0;
  for(i=0;i<MAXOBJECTS; i++){
    if( *(gobjects+i)!=NULL){
      used += 1;
    }
  }
  printf("%d Objectct allocated, out of %d\n", used, MAXOBJECTS);
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

void  remove_entry(Object* entry){
  int i;
  for(i=0;i<MAXOBJECTS; i++){
    if( *(gobjects+i)== entry){
      *(gobjects+i) = NULL;
      return;
    }
  }
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
  remove_entry(self);
  free((void*)self);
}


Object* gstack;

void init_stack(){
  gstack = NULL;
}

void push(Object* obj){
  if (gdebug){
    print_obj(obj, "pushing");
  }
  obj->fPrev = gstack;
  gstack = obj;
}

Object* pop(void){
  Object* r;
  r = gstack;
  if (gdebug){
    print_obj(r, "popping");
  }
  gstack = gstack->fPrev;
  return r;
}


void print_stack(void){
  char buf[10];
  int depth = 0;
  Object* peeker;
  peeker = gstack;
  printf("===== stack dump start ====\n");
  while(peeker){
    sprintf(buf, "(depth=%i)", depth);
    print_obj(peeker, buf);
    depth += 1;
    peeker = peeker->fPrev;
  }
  printf("===== stack dump end ====\n");
}

void mark_tree(Object* obj){

  if(obj->fObject1){
    mark_tree(obj->fObject1);
  }
  if(obj->fObject2){
    mark_tree(obj->fObject2);
  }

  obj->fGeneration = ggeneration;
  if(gdebug){
    printf("marking %p %d\n", obj, obj->fGeneration);
  }
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

  if(gdebug){
    printf("generation %d\n", ggeneration);
  }
  for(i=0; i<MAXOBJECTS; i++){
    obj = *(gobjects+i);
    if(obj){
      if(gdebug){
        printf("checking %p(%p) %d\n", obj, gobjects+i ,obj->fGeneration);
      }
      if(ggeneration != obj->fGeneration){
        if(gdebug){
          printf("deleting %p\n", obj);
        }
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
  r->fName = "quote";
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
  r->fName = "print";
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
  r->fName = "identity";
  return r;
}


Object* _k1(Object* self, Object* other){
  return self->fObject1;
}


Object* _constant_function(Object* self, Object* other){
  Object* k1;
  k1 = NewObject();
  k1->fProc = _k1;
  k1->fName = "k1";
  k1->fObject1 = other;
  return k1;
}

Object* constant_function(){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = NewObject();
  r->fProc = &_constant_function;
  r->fName = "constant_function";
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
  s2->fName = "s2";
  return s2;
}


Object* _generalized_evaluation(Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = NewObject();
  s1->fProc = &_s1;
  s1->fObject1 = other;
  s1->fName = "s1";
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
  r->fName = "generalized_evaluation";
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
      if(gdebug)
        print_stack();
      run_once();
      mark();
      sweep();
      if(gdebug)
        mem_stat();
    }
    xs++;
  }
  while (runnable()){
    if(gdebug)
      print_stack();
    run_once();
    mark();
    sweep();
    if(gdebug)
      mem_stat();
  }
  gstack = NULL;
  mark();
  sweep();

  return 0;
}
