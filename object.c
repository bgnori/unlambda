#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object.h"

#define false 0
#define true 1



int gdebug = 0;
void print_obj(Object* obj, char* msg){
  printf("%s %s %p\n", msg, obj->fName, obj);
}


Object* call(World* world, Object* self, Object* other){
  if(gdebug){
      print_obj(self, "operator");
      print_obj(other, "operand");
  }
  return self->fProc(world, self, other);
}


void InitWorld(World* world){
  return;
}


void World_start(World* world, int memsize){
  int i;
  Memory* m;

  m = (Memory*)NewObject(world, sizeof(Memory));

  m->fObjects = (Object**)malloc(sizeof(Object*)*memsize);
  m->fSize = memsize;
  if(gdebug){
    printf("start/end: %p %p\n", m->fObjects, m->fObjects+m->fSize);
    /*
     * start/end: 0x4c2d040 0x4c2ef80
     */
  }
  for(i=0;i<m->fSize; i++){
    *(m->fObjects+i) = NULL;
  }
  m->fGeneration = 0;
  world->fMemory = m; 
}


void World_stop(World* world){
  Memory* m;
  m = (Memory*)(world->fMemory);
  free((void*)(m->fObjects));
  free((void*)m);
}


void mem_stat(World *world){
  int i;
  int used;
  used = 0;
  for(i=0;i< world->fMemory->fSize; i++){
    if( *(world->fMemory->fObjects+i)!=NULL){
      used += 1;
    }
  }
  printf("%d Object allocated, out of %d\n", used, world->fMemory->fSize);
}


void make_new_entry(World *world, Object* obj){
  int i;
  for(i=0;i<world->fMemory->fSize; i++){
    if( *(world->fMemory->fObjects+i)==NULL){
      *(world->fMemory->fObjects+i) = obj;
      return;
    }
  }
  assert(false);
}

void  remove_entry(World* world, Object* entry){
  int i;
  for(i=0;i<world->fMemory->fSize; i++){
    if( *(world->fMemory->fObjects+i)== entry){
      *(world->fMemory->fObjects+i) = NULL;
      return;
    }
  }
}


Object* NewObject(World* world, int size){
  Object* r;
  r = (Object*)malloc(sizeof(Object));
  make_new_entry(world, r);
  r->fPrev = NULL;
  r->fObject1 = NULL;
  r->fObject2 = NULL;
  r->fGeneration = -1;
  return r;
}

void DeleteObject(World* world, Object* self){
  remove_entry(world, self);
  free((void*)self);
}


Object* gstack;

void init_stack(World* world){
  gstack = NULL;
}

void push(World* world, Object* obj){
  if (gdebug){
    print_obj(obj, "pushing");
  }
  obj->fPrev = gstack;
  gstack = obj;
}

Object* pop(World* world){
  Object* r;
  r = gstack;
  if (gdebug){
    print_obj(r, "popping");
  }
  gstack = gstack->fPrev;
  return r;
}


void print_stack(World* world){
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

void mark_tree(World* world, Object* obj){

  if(obj->fObject1){
    mark_tree(world, obj->fObject1);
  }
  if(obj->fObject2){
    mark_tree(world, obj->fObject2);
  }

  obj->fGeneration = world->fMemory->fGeneration;
  if(gdebug){
    printf("marking %p %d\n", obj, obj->fGeneration);
  }
}

void mark(World* world){
  Object* obj;

  world->fMemory->fGeneration += 1;
  obj = gstack;
  while(obj){
    mark_tree(world, obj);
    obj = obj->fPrev;
  }
}

void sweep(World* world){
  int i;
  Object* obj;

  if(gdebug){
    printf("generation %d\n", world->fMemory->fGeneration);
  }
  for(i=0; i<world->fMemory->fSize; i++){
    obj = *(world->fMemory->fObjects+i);
    if(obj){
      if(gdebug){
        printf("checking %p(%p) %d\n", obj, world->fMemory->fObjects+i ,obj->fGeneration);
      }
      if(world->fMemory->fGeneration != obj->fGeneration){
        if(gdebug){
          printf("deleting %p\n", obj);
        }
        DeleteObject(world, obj);
        obj = NULL;
      }
    }
  }
}


int runnable(World* world){
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



Object* quote(World* world){
  Object* r;
  r = NewObject(world, sizeof(Object));
  r->fProc = NULL;
  r->fObject1 = NULL;
  r->fName = "quote";
  return r;
}


Object* _print(World* world, Object* self, Object* other){
  printf("%c", self->fChar);
  return other;
}

Object* print(World* world, char x){
  Object* r;
  r = NewObject(world, sizeof(Object));
  r->fProc = &_print;
  r->fChar = x;
  r->fName = "print";
  return r;
}


Object* _identity(World* world, Object* self, Object* other){
  return other;
}

Object* identity(World* world){
  Object* r;
  r = NewObject(world, sizeof(Object));
  r->fProc = &_identity;
  r->fObject1 = NULL;
  r->fName = "identity";
  return r;
}


Object* _k1(World* world, Object* self, Object* other){
  return self->fObject1;
}


Object* _constant_function(World* world, Object* self, Object* other){
  Object* k1;
  k1 = NewObject(world, sizeof(Object));
  k1->fProc = _k1;
  k1->fName = "k1";
  k1->fObject1 = other;
  return k1;
}

Object* constant_function(World* world){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = NewObject(world, sizeof(Object));
  r->fProc = &_constant_function;
  r->fName = "constant_function";
  return r;
}


Object* _s2(World* world, Object* self, Object* other){
  /* self == s2, other == z */
  Object* x; Object* y; Object*z;
  Object* xz; Object* yz;
  Object* r;
  z = other;
  y = self->fObject1;
  x = self->fObject2->fObject1;
  xz = call(world, x, z);
  yz = call(world, y, z);
  r = call(world, xz, yz);

  return r;
}


Object* _s1(World* world, Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = NewObject(world, sizeof(Object));
  s2->fProc = &_s2;
  s2->fObject1 = other;
  s2->fObject2 = self; /* may be it is better idea to reference x, instead of s1 */
  s2->fName = "s2";
  return s2;
}


Object* _generalized_evaluation(World* world, Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = NewObject(world, sizeof(Object));
  s1->fProc = &_s1;
  s1->fObject1 = other;
  s1->fName = "s1";
  return s1;
}

Object* generalized_evaluation(World* world){
  /*
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = NewObject(world, sizeof(Object));
  r->fProc = &_generalized_evaluation;
  r->fObject1 = NULL;
  r->fName = "generalized_evaluation";
  return r;
}


void run_once(World* world){
  Object *operand, *operator, *q, *r;

  operand = pop(world);
  operator = pop(world);
  q = pop(world);
  r = call(world, operator, operand);
  push(world, r);
}



void World_eval(World* world, char* xs){
  char x;

  init_stack(world);
  while (*xs){
    x = *xs;

    switch(x){
      case '`':
        push(world, quote(world));
        break;

      case '.':
        xs++;
        push(world, (print(world, *xs)));
        break;

      case 'r':
        push(world, (print(world, '\n')));
        break;

      case 'i':
        push(world, identity(world));
        break;

      case 'k':
        push(world, constant_function(world));
        break;

      case 's':
        push(world, generalized_evaluation(world));
        break;

      default:
        break;
    }
    while (runnable(world)){
      if(gdebug)
        print_stack(world);
      run_once(world);
      mark(world);
      sweep(world);
      if(gdebug)
        mem_stat(world);
    }
    xs++;
  }
  while (runnable(world)){
    if(gdebug)
      print_stack(world);
    run_once(world);
    mark(world);
    sweep(world);
    if(gdebug)
      mem_stat(world);
  }
  gstack = NULL;
  mark(world);
  sweep(world);

  return;
}

