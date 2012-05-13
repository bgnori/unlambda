#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object.h"


World* gWorld = NULL;

#define XNewObject(TYPE) ((TYPE*)_XNewObject((sizeof(TYPE))))
void make_new_entry(Object* obj);
void  remove_entry(Object* entry);

Object* _XNewObject(int size){
  Object* r;
  r = (Object*)malloc(sizeof(Object));
  r->fOne.uObject = NULL;
  r->fTwo.uObject = NULL;
  return r;
}

Object* NewInterger(int v){
  Object* obj;

  obj = NewObject(Object);
  obj->fOne.uInteger = v;
  obj->fTwo.uInteger = 0;
  return obj;
}


void XDeleteObject(Object* self){
  free((void*)self);
}

void Object_print(Object* obj, char* msg){
  printf("%s %s %p\n", msg, obj->fName, obj);
}

World* CreateWorld(int memsize){
  World* w;
  Memory* m;

  w = XNewObject(World);

  World_setDebug(w, true);

  m = CreateMemory(w, memsize);

  Memory_NthEntry(m, 0)->fTarget = (Object*)w;
  Memory_NthEntry(m, 0)->fGeneration = -1;
  World_setMemory(w, m);

  return w;
}

void setWorld(World* w){
  gWorld = w;
}

World* getWorld(void){
  return gWorld;
}

Memory* CreateMemory(World* world, int memsize){
  Memory* m;
  void* chunk;
  MemoryEntry** start;
  MemoryEntry** end;
  Object* generation;

  m = XNewObject(Memory);

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

  generation = NewInterger(0);

  Memory_setNthEntry(m, 1, (Object*)m, -1);
  Memory_setNthEntry(m, 2, generation, -1);
  return m;
}


void Memory_setNthEntry(Memory* memory, int n, Object* target, int generation){
  MemoryEntry entry;

  entry.fTarget = target;
  entry.fGeneration = generation;
  *(*(Memory_getEntries(memory)) + n)  = entry;
}

Object* Memory_getNthObject(Memory* memory, int n){
  return Memory_NthEntry(memory, n)->fTarget;
}

void Memory_setNthObject(Memory* memory, int n, Object* obj){
  (*(Memory_getEntries(memory)) + n)->fTarget = obj;
  (*(Memory_getEntries(memory)) + n)->fGeneration = 0; /*FIXME*/
}


int Memory_getGeneration(Memory* m){
  Object* i;
  i = Memory_NthEntry(m, 2)->fTarget;
  return i->fOne.uInteger;
}

void Memory_incGeneration(Memory* self){
  Object* i;
  i = Memory_NthEntry(self, 2)->fTarget;
  i->fOne.uInteger+=1;
}


void DeleteMemory(Memory* self){
  if(World_getDebug(getWorld())){
    Memory_stat(self);
  }
  free((void*)Memory_getEntries(self));
  XDeleteObject((Object*)self);
}


void DeleteWorld(World* world){
  DeleteMemory(World_getMemory(world));
  XDeleteObject((Object*)world);
}


Object* _NewObject(int size){
  Object* r;
  r = _XNewObject(size);
  make_new_entry(r);
  return r;
}

void _DeleteObject(Object* obj){
  remove_entry(obj);
  XDeleteObject(obj);
}

void Memory_stat(Memory* self){
  int i;
  int used;
  used = 0;
  for(i=0;i< Memory_getSize(self); i++){
    if(Memory_NthEntry(self, i)->fTarget != NULL){
      used += 1;
    }
  }
  printf("%d Object allocated, out of %d\n", used, Memory_getSize(self));
}


void make_new_entry(Object* obj){
  Memory* m;
  MemoryEntry* e;
  m = getMemory();

  int i;
  for(i=0;i<Memory_getSize(m); i++){
    e = Memory_NthEntry(m, i);

    if(e->fTarget ==NULL){
      e->fTarget = obj;
      return;
    }
  }
  assert(false);
}

void remove_entry(Object* obj){
  Memory* memory;
  MemoryEntry* entry;
  int i;
  memory = World_getMemory(getWorld());

  for(i=0;i<Memory_getSize(memory); i++){
    entry = Memory_NthEntry(memory, i);
    if(entry->fTarget== obj){
      entry->fTarget = NULL;
      return;
    }
  }
}

Stack * NewStack(void){
  Stack* r;
  r = NewObject(Stack);
  Stack_setTopNode(r, NULL);
  return r;
}

void Stack_push(Stack* stack, Object* obj){
  StackNode* x;
  StackNode* y;

  if (World_getDebug(getWorld())){
    Object_print(obj, "pushing");
  }
  x = Stack_getTopNode(stack);
  y = NewObject(StackNode);

  StackNode_setNext(y, x);
  StackNode_setData(y, obj);
  Stack_setTopNode(stack, y);
}

Object* Stack_pop(Stack* stack){
  Object* r;
  StackNode* n;

  if (World_getDebug(getWorld())){
    Object_print(r, "popping");
  }

  n = Stack_getTopNode(stack);
  r = StackNode_getData(n); 
  Stack_setTopNode(stack, StackNode_getNext(n));

  DeleteObject(n); 
  return r;
}


void Stack_print(Stack* stack){
  char buf[10];
  int depth = 0;
  StackNode* peeker;
  peeker = Stack_getTopNode(stack);

  printf("===== stack dump start ====\n");
  while(peeker){
    sprintf(buf, "(depth=%i)", depth);
    Object_print(StackNode_getData(peeker), buf);
    depth += 1;
    peeker = StackNode_getNext(peeker);
  }
  printf("===== stack dump end ====\n");
}

void mark_tree(Object* obj){

  if(obj->fOne.uObject){
    mark_tree(obj->fOne.uObject);
  }
  if(obj->fTwo.uObject){
    mark_tree(obj->fTwo.uObject);
  }

  /* FIXME
  //obj->fGeneration = World_getMemory(world)->fGeneration;
  //FIXME
  //Memory

  if(World_getDebug(getWorld())){
    printf("marking %p %d\n", obj, obj->fGeneration);
  }
  */
}

void Memory_mark(Memory* self, Stack* stack);
  StackNode* peeker;

  Memory_incGeneration(self);
  peeker = Stack_getTopNode(stack);
  while(peeker){
    mark_tree(StackNode_getData(peeker));
    peeker = StackNode_getNext(peeker);
  }
}

void Memory_sweep(Memory* self){
  int i;
  Object* obj;

  if(World_getDebug(getWorld())){
    printf("generation %d\n", Memory_getGeneration(self));
  }
  for(i=0; i<Memory_getSize(self); i++){
    obj = Memory_getNthObject(self, i);
    if(obj){
      if(World_getDebug(getWorld())){
        printf("checking %p %d\n", obj, obj->fGeneration);
      }
      if(!= obj->fGeneration){
        if(World_getDebug(world)){
          printf("deleting %p\n", obj);
        }
        World_deleteObject(world, obj);
        obj = NULL;
      }
    }
  }
}


