#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object.h"

#define false 0
#define true 1


typedef struct TObjectField ObjectField;
struct TObjectField {
  Object* uObject;
  int uInteger;
  char uChar;
  char* uStringPtr;
  int uStringLen;
};

struct TObject {
  Func fProc;
  char* fName;
  ObjectField fOne; 
  ObjectField fTwo; 
};
Object* _NewObject(int size);


#define World_newChar


struct TWorld {
  Object fBase;
  int fDummy;
};
Object* World_newObject(World* world, int size);
#define World_setMemory(world, x) ((((Object*)world)->fOne.uObject) = (Object*)(x))
#define World_getMemory(world) ((Memory)(((Object*)world)->fOne.uObject))
#define World_setDebug(world, x) ((((Object*)world)->fTwo.uInteger) = (x))
#define World_getDebug(world) ((((Object*)world)->fTwo.uInteger))


Memory* CreateMemory(World* world, int memsize);
typedef struct TMemoryEntry MemoryEntry;
struct TMemoryEntry {
  Object* fTarget;
  int fGeneration;
};

struct TMemory{
  Object fBase;
  int fDummy;
};
#define Memory_setEntries(memory, x) ((((Object*)(memory))->fOne.uObject) = (Object*)(x))
#define Memory_getEntries(memory) ((MemoryEntry**)(((Object*)(memory))->fOne.uObject))
#define Memory_setSize(memory, x)((((Object*)(memory))->fTwo.uInteger) = (x))
#define Memory_getSize(memory) ((((Object*)(memory))->fTwo.uInteger))

MemoryEntry Memory_getNth(Memory* memory, int n);
void Memory_setNth(Memory* memory, int n, Object* target, int generation);


void Stack_push(World* world, Object* object);
void Stack_pop(World* world);
typedef struct TStack Stack;
struct TStack{
  Object fBase;
  int fDummy;
};

void UnlambdaEval_eval(World* world, char* xs);
struct TUnlambdaEval{
  Object fBase;
  Stack* fCallStack;
};



#define NewObject(TYPE) ((TYPE*)_NewObject((sizeof(TYPE))))
Object* _NewObject(int size){
  Object* r;
  r = (Object*)malloc(sizeof(Object));
  r->fOne.uObject = NULL;
  r->fTwo.uObject = NULL;
  return r;
}

Object* World_newInterger(int v){
  Object* obj;

  obj = NewObject(Object);
  obj->fOne.uInteger = v;
  obj->fTwo.uInteger = 0;
  return obj;
}





#define DeleteObject(self) _DeleteObject((Object*)(self))
void _DeleteObject(Object* self){
  free((void*)self);
}

void Object_print(Object* obj, char* msg){
  printf("%s %s %p\n", msg, obj->fName, obj);
}


Object* World_call(World* world, Object* self, Object* other){
  if(World_getDebug(world)){
      Object_print(self, "operator");
      Object_print(other, "operand");
  }
  return self->fProc(world, self, other);
}



World* CreateWorld(int memsize){
  World* w;
  Memory* m;
  w = NewObject(World);
  World_setDebug(w, true);

  m = CreateMemory(w, memsize);

  Memory_setNth(m, 0, w);
  World_setMemory(w, m);
  return w;
}


Memory* CreateMemory(World* world, int memsize){
  Memory* m;
  void* chunk;
  MemoryEntry** start;
  MemoryEntry** end;
  Object* generation;

  m = NewObject(Memory);

  chunk = malloc(sizeof(Object*)*memsize);
  Memory_setEntries(m, chunk);
  Memory_setSize(m, memsize);

  start = Memory_getEntries(m);
  end = start + Memory_getSize(m);
  if(World_getDebug(world)){
    printf("start/end: %p %p\n", start, end);
  }
  for(;start<end; start++){
    start = NULL;
  }

  generation = World_newInterger(0);

  Memory_setNth(m, 1, m, -1);
  Memory_setNth(m, 2, generation, -1);
  return m;
}

MemoryEntry Memory_getNth(Memory* memory, int n){
  return *(*(Memory_getEntries(memory)) + n);
}

void Memory_setNth(Memory* memory, int n, Object* target, int generation){
  MemoryEntry entry;

  entry.fTarget = target;
  entry.fGeneration = generation;
  *(*(Memory_getEntries(memory)) + n)  = entry;
}


void DeleteUnlambdaEval(UnlambdaEval* unlambda){
  gstack = NULL;
  mark(world);
  sweep(world);
}

void DeleteMemory(Memory* memory){

  free((void*)(m->fObjects));
  DeleteObject(m);
}


void DeleteWorld(World* world){
  DeleteMemory(World_Memory(world));
  DeleteObject(world);
}


Object* World_newObject(World* world, int size){
  Object* r;
  r = _NewObject(size);
  make_new_entry(world, r);
  return r;
}

void World_deleteObject(World* world, Object* obj){
  remove_entry(world, obj);
  DeleteObject(obj);
}



UnlambdaEval* World_newUnlambdaEval(World* world){
  init_stack(world);
}

void World_deleteUnlambdaEval(World* world){


}


void mem_stat(World *world){
  int i;
  int used;
  used = 0;
  for(i=0;i< World_Memory(world)->fSize; i++){
    if( *(World_Memory(world)->fObjects+i)!=NULL){
      used += 1;
    }
  }
  printf("%d Object allocated, out of %d\n", used, World_Memory(world)->fSize);
}


void make_new_entry(World *world, Object* obj){
  int i;
  for(i=0;i<World_Memory(world)->fSize; i++){
    if( *(World_Memory(world)->fObjects+i)==NULL){
      *(World_Memory(world)->fObjects+i) = obj;
      return;
    }
  }
  assert(false);
}

void  remove_entry(World* world, Object* entry){
  int i;
  for(i=0;i<World_Memory(world)->fSize; i++){
    if( *(World_Memory(world)->fObjects+i)== entry){
      *(World_Memory(world)->fObjects+i) = NULL;
      return;
    }
  }
}



Object* gstack;

void init_stack(World* world){
  gstack = NULL;
}

void push(World* world, Object* obj){
  if (World_getDebug(world)){
    Object_print(obj, "pushing");
  }
  obj->fPrev = gstack;
  gstack = obj;
}

Object* pop(World* world){
  Object* r;
  r = gstack
  if (World_getDebug(world)){
    Object_print(r, "popping");
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
    Object_print(peeker, buf);
    depth += 1;
    peeker = peeker->fPrev;
  }
  printf("===== stack dump end ====\n");
}

void mark_tree(World* world, Object* obj){

  if(obj->fOne.uObject){
    mark_tree(world, obj->fOne.uObject);
  }
  if(obj->fTwo.uObject){
    mark_tree(world, obj->fTwo.uObject);
  }

  obj->fGeneration = World_Memory(world)->fGeneration;
  if(World_getDebug(world)){
    printf("marking %p %d\n", obj, obj->fGeneration);
  }
}

void mark(World* world){
  Object* obj;

  World_Memory(world)->fGeneration += 1;
  obj = gstack;
  while(obj){
    mark_tree(world, obj);
    obj = obj->fPrev;
  }
}

void sweep(World* world){
  int i;
  Object* obj;

  if(World_getDebug(world)){
    printf("generation %d\n", World_Memory(world)->fGeneration);
  }
  for(i=0; i<World_Memory(world)->fSize; i++){
    obj = *(World_Memory(world)->fObjects+i);
    if(obj){
      if(World_getDebug(world)){
        printf("checking %p(%p) %d\n", obj, World_Memory(world)->fObjects+i ,obj->fGeneration);
      }
      if(World_Memory(world)->fGeneration != obj->fGeneration){
        if(World_getDebug(world)){
          printf("deleting %p\n", obj);
        }
        World_deleteObject(world, obj);
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
  r = World_newObject(world, sizeof(Object));
  r->fProc = NULL;
  r->fOne.uObject = NULL;
  r->fName = "quote";
  return r;
}


Object* _print(World* world, Object* self, Object* other){
  printf("%c", self->fChar);
  return other;
}

Object* print(World* world, char x){
  Object* r;
  r = World_newObject(world, sizeof(Object));
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
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_identity;
  r->fOne.uObject = NULL;
  r->fName = "identity";
  return r;
}


Object* _k1(World* world, Object* self, Object* other){
  return self->fOne.uObject;
}


Object* _constant_function(World* world, Object* self, Object* other){
  Object* k1;
  k1 = World_newObject(world, sizeof(Object));
  k1->fProc = _k1;
  k1->fName = "k1";
  k1->fOne.uObject = other;
  return k1;
}

Object* constant_function(World* world){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = World_newObject(world, sizeof(Object));
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
  y = self->fOne.uObject;
  x = self->fTwo.uObject->fOne.uObject;
  xz = World_call(world, x, z);
  yz = World_call(world, y, z);
  r = World_call(world, xz, yz);

  return r;
}


Object* _s1(World* world, Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = World_newObject(world, sizeof(Object));
  s2->fProc = &_s2;
  s2->fOne.uObject = other;
  s2->fTwo.uObject = self; /* may be it is better idea to reference x, instead of s1 */
  s2->fName = "s2";
  return s2;
}


Object* _generalized_evaluation(World* world, Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = World_newObject(world, sizeof(Object));
  s1->fProc = &_s1;
  s1->fOne.uObject = other;
  s1->fName = "s1";
  return s1;
}

Object* generalized_evaluation(World* world){
  /*
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_generalized_evaluation;
  r->fOne.uObject = NULL;
  r->fName = "generalized_evaluation";
  return r;
}


void run_once(World* world){
  Object *operand, *operator, *q, *r;

  operand = pop(world);
  operator = pop(world);
  q = pop(world);
  r = World_call(world, operator, operand);
  push(world, r);
}



void  World_eval(World* world, char* xs){
  char x;

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
      if(World_getDebug(world))
        print_stack(world);
      run_once(world);
      mark(world);
      sweep(world);
      if(World_getDebug(world))
        mem_stat(world);
    }
    xs++;
  }
  while (runnable(world)){
    if(World_getDebug(world))
      print_stack(world);
    run_once(world);
    mark(world);
    sweep(world);
    if(World_getDebug(world))
      mem_stat(world);
  }

  return;
}

