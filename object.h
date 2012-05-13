
#ifndef OBJECT_H
#define OBJECT_H

#define false 0
#define true 1

typedef struct TObject Object;

typedef struct TWorld World;
typedef struct TMemory Memory;
typedef struct TStack Stack;

typedef Object*(*Func)(Object* self, Object* other);


World* getWorld(void);
void setWorld(World* world);
World* CreateWorld(int memsize);
void DeleteWorld(World* world);

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
#define NewObject(TYPE) ((TYPE*)_NewObject((sizeof(TYPE))))
Object* _NewObject(int size);
#define DeleteObject(self) _DeleteObject((Object*)(self))
void _DeleteObject(Object* self);

Object* NewInterger(int v);
void Object_print(Object* obj, char* msg);



struct TWorld {
  Object fBase;
  int fDummy;
};
Object* World_newObject(World* world, int size);
#define World_setMemory(world, x) ((((Object*)world)->fOne.uObject) = (Object*)(x))
#define World_getMemory(world) ((Memory*)(((Object*)world)->fOne.uObject))
#define World_setDebug(world, x) ((((Object*)world)->fTwo.uInteger) = (x))
#define World_getDebug(world) ((((Object*)world)->fTwo.uInteger))

#define getMemory() World_getMemory(getWorld())


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

#define Memory_NthEntry(self, n) (*(Memory_getEntries((self))) + (n))
void Memory_setNthEntry(Memory* memory, int n, Object* target, int generation);
Object* Memory_getNthObject(Memory* memory, int n);
void Memory_setNthObject(Memory* memory, int n, Object* obj);


void Memory_incGeneration(Memory* self);
int Memory_getGeneration(Memory* m);

void Memory_mark(Memory* self, Stack* stack);
void Memory_sweep(Memory* self);
void Memory_stat(Memory* self);



struct TStack{
  Object fBase;
  int fDummy;
};
Stack* NewStack();
#define Stack_getTopNode(stack) ((StackNode*)((stack)->fBase.fOne.uObject))
#define Stack_setTopNode(stack, node) (((stack)->fBase.fOne.uObject) = (Object*)(node))
void Stack_push(Stack* stack, Object* item);
Object* Stack_pop(Stack* stack);
void Stack_print(Stack* stack);


typedef struct TStackNode StackNode;
struct TStackNode {
  Object fBase;
  int fDummy;
};
#define StackNode_getNext(node) ((StackNode*)((node)->fBase.fOne.uObject))
#define StackNode_setNext(node, next) (((node)->fBase.fOne.uObject) = (Object*)(next))
#define StackNode_getData(node) ((Object*)((node)->fBase.fTwo.uObject))
#define StackNode_setData(node, data) (((node)->fBase.fTwo.uObject) = (Object*)(data))



#endif /* OBJECT_H */

