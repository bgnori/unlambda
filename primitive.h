#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#define false 0
#define true 1
#define NULL ((void*)0)


typedef struct TPrimitive Primitive;
typedef Primitive*(*Func)(Primitive* data, Primitive* x);
typedef struct TBinaryNode BinaryNode;
typedef struct TStack Stack;
typedef struct TList List;
typedef struct TString String;
typedef struct TChar Char;
typedef struct TChunk Chunk;
typedef struct TFunctionoid Functionoid;
typedef struct TPrimitiveField PrimitiveField;


/*
 *
 * Primitive
 *
 */

struct TPrimitiveField {
  Primitive* uPrimitive;
  Stack* uStack;
  BinaryNode* uBinaryNode;
  int uInteger;
  char uChar;
  char* uStringPtr;
  int uStringLen;
  void* uChunkPtr;
  int uChunkLen;
  Func uProc;
};

typedef enum TPrimitiveMetaEnum PrimitiveMetaEnum;
enum TPrimitiveMetaEnum {
  PRIMITIVEPTR,
  STACKPTR,
  STACKNODEPTR,
  INTEGER,
  CHAR,
  STRINGPTR,
  STRINGLEN,
  CHUNKPTR,
  CHUNKLEN,
  PROC,
  PrimitiveMetaEnums
};

typedef struct TPrimitiveMeta PrimitiveMeta;
struct TPrimitiveMeta {
    PrimitiveMetaEnum fMetaOne;
    PrimitiveMetaEnum fMetaTwo;
    char fGC;
};

struct TPrimitive {
  PrimitiveMeta fMeta;
  PrimitiveField fOne; 
  PrimitiveField fTwo; 
};

#define XNewPrimitive(TYPE) ((TYPE*)_XNewPrimitive((sizeof(TYPE))))
Primitive* _XNewPrimitive(int size);
#define XDeletePrimitive(self) _XDeletePrimitive((Primitive*)(self))
void _XDeletePrimitive(Primitive* p);

/* hooks! for gc, etc */
void onNewPrimitive(Primitive* p);
void onDeletePrimitive(Primitive* p);

#define NewPrimitive(TYPE) ((TYPE*)_NewPrimitive((sizeof(TYPE))))
Primitive* _NewPrimitive(int size);
#define DeletePrimitive(self) _DeletePrimitive((Primitive*)(self))
void _DeletePrimitive(Primitive* self);
void Primitive_print(Primitive* self);


/*
 *
 * BinaryNode for implementing Stack and list.
 *
 */
struct TBinaryNode {
  Primitive fBase;
};
#define NewBinaryNode() NewPrimitive(BinaryNode)
#define DeleteBinaryNode(self) DeletePrimitive((Primitive*)(self))

#define BinaryNode_getNext(self) ((self)->fBase.fOne.uBinaryNode)
#define BinaryNode_setNext(self, next) ((self)->fBase.fOne.uBinaryNode = (next))
#define BinaryNode_getData(self) ((self)->fBase.fTwo.uPrimitive)
#define BinaryNode_setData(self, data) ((self)->fBase.fTwo.uPrimitive = (Primitive*)(data))


/*
 *
 * Stack 
 *
 */

struct TStack{
  Primitive fBase;
};

Stack* NewStack(void);
void DeleteStack(Stack* self);
#define Stack_getTopNode(self) ((self)->fBase.fOne.uBinaryNode)
#define Stack_setTopNode(self, node) ((self)->fBase.fOne.uBinaryNode = (node))
void Stack_push(Stack* self, Primitive* item);
Primitive* Stack_pop(Stack* self);
void Stack_print(Stack* self);


/*
 *
 * List 
 *
 */
struct TList{
  Primitive fBase;
};

List* NewList(void);
void DeleteList(List* self);
#define List_getHeadNode(self) ((self)->fBase.fOne.uBinaryNode)
#define List_setHeadNode(self, node) ((self)->fBase.fOne.uBinaryNode = (node))
#define List_getTailNode(self) ((self)->fBase.fTwo.uBinaryNode)
#define List_setTailNode(self, node) ((self)->fBase.fTwo.uBinaryNode = (node))
void List_append(List* stack, Primitive* item);
Primitive* List_pop(List* stack);
void List_print(List* stack);


/*
 *
 * Functionoid
 *
 * not closure, * not fuction, not functor, so Functionoid was only choice.
 *
 */
struct TFunctionoid{
  Primitive fBase;
};
Functionoid* NewFunctionoid(Func* f, Primitive* data);
Primitive* Functionoid_call(Functionoid* self, Primitive *x);


/*
 *
 * Char
 *
 */
struct TChar{
  Primitive fBase;
};
Char* NewChar(char c);
void DeleteChar(Char* self);
char Char_getChar(Char* self);


/*
 *
 * String
 *
 */
struct TString {
  Primitive fBase;
};
String* NewString(char*s, int len);
void DeleteString(String* self);
char* String_getCString(String* self); 
/* some string may contain '\0'. what should i do? */
/* This is NOT okay to write to this string. */


/*
 *
 * Chunk
 *
 */
struct TChunk {
  Primitive fBase;
};
Chunk* NewChunk(int count);
void DeleteChunk(Chunk* self);
Primitive* Chunk_getNth(Chunk* self, int n);
void Chunk_setNth(Chunk* self, int n, Primitive* p);



#endif /* PRIMITIVE_H */
