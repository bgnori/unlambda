
#ifndef OBJECT_H
#define OBJECT_H


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
  char* fName;
};

#endif /* OBJECT_H */
