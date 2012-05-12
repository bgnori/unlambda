#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object.h"


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

  Memory_setNth(m, 0, (Object*)w, -1);
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

  Memory_setNth(m, 1, (Object*)m, -1);
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


