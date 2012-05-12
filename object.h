
#ifndef OBJECT_H
#define OBJECT_H

typedef struct TObject Object;
typedef struct TInteger Integer;

typedef struct TWorld World;
typedef struct TMemory Memory;
typedef struct TCallStack CallStack;
typedef struct TUnlambdaEval UnlambdaEval;

typedef Object*(*Func)(World* world, Object* self, Object* other);

World* CreateWorld(int memsize);

void DeleteWorld(World* world);
void World_unlambda(World* world, char* xs);


#endif /* OBJECT_H */

