
#ifndef OBJECT_H
#define OBJECT_H

typedef struct TObject Object;
typedef struct TMemory Memory;
typedef struct TWorld World;

typedef Object*(*Func)(World* world, Object* self, Object* other);

struct TObject {
  Func fProc;
  char* fName;
  Object* fPrev;
  int fGeneration;
  Object * fObject1;
  Object * fObject2;
  char fChar;
};

Object* NewObject(World* world, int size);


#define World(obj) ((World*)(obj))
struct TWorld {
  Object fBase;
  int fDummy;
  Memory * fMemory;
};


void InitWorld(World* world);
void World_start(World* world, int memsize);
void World_stop(World* world);
void World_eval(World* world, char* xs);


#define Memory(obj) ((Memory*)(obj))
struct TMemory{
  Object fBase;
  int fGeneration;
  int fSize;
  Object** fObjects;
};



#endif /* OBJECT_H */

