
#ifndef OBJECT_H
#define OBJECT_H

#define false 0
#define true 1

typedef struct TObject Object;

typedef struct TWorld World;
typedef struct TMemory Memory;
typedef struct TCallStack CallStack;
typedef struct TUnlambdaEval UnlambdaEval;

typedef Object*(*Func)(World* world, Object* self, Object* other);

World* CreateWorld(int memsize);

void DeleteWorld(World* world);
Object* World_call(World* world, Object* self, Object* other);
void World_unlambda(World* world, char* xs);


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
#define DeleteObject(self) _DeleteObject((Object*)(self))
void _DeleteObject(Object* self);

Object* World_newInterger(int v);
void Object_print(Object* obj, char* msg);


#define World_newChar TBI


struct TWorld {
  Object fBase;
  int fDummy;
};
Object* World_newObject(World* world, int size);
#define World_setMemory(world, x) ((((Object*)world)->fOne.uObject) = (Object*)(x))
#define World_getMemory(world) ((Memory*)(((Object*)world)->fOne.uObject))
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

typedef struct TStack Stack;
struct TStack{
  Object fBase;
  int fDummy;
};
void Stack_push(Stack* stack, Object* item);
Object* Stack_pop(Stack* stack);

void UnlambdaEval_eval(World* world, char* xs);
struct TUnlambdaEval{
  Object fBase;
  Stack* fCallStack;
};


#define NewObject(TYPE) ((TYPE*)_NewObject((sizeof(TYPE))))

#endif /* OBJECT_H */

