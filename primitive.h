#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#define false 0
#define true 1


typedef struct TPrimitive Primitive;
typedef Primitive*(*BinaryFunc)(Primitive* x, Primitive* y);
typedef struct TStack Stack;
typedef struct TStackNode StackNode;
typedef struct TList List;
typedef struct TListNode ListNode;

typedef struct TPrimitiveField PrimitiveField;


/*
 *
 * Primitive
 *
 */

struct TPrimitiveField {
  Primitive* uPrimitive;
  Stack* uStack;
  StackNode* uStackNode;
  int uInteger;
  char uChar;
  char* uStringPtr;
  int uStringLen;
  void* uChunkPtr;
  int uChunkLen;
  BinaryFunc uProc;
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


/*
 *
 * Stack and Stack Node
 *
 */

struct TStack{
  Primitive fBase;
};

Stack* NewStack(void);
void DeleteStack(Stack* self);
#define Stack_getTopNode(self) ((self)->fBase.fOne.uStackNode)
#define Stack_setTopNode(self, node) ((self)->fBase.fOne.uStackNode = (node))
void Stack_push(Stack* self, Primitive* item);
Primitive* Stack_pop(Stack* self);
void Stack_print(Stack* self);

struct TStackNode {
  Primitive fBase;
};

#define NewStackNode() NewPrimitive(StackNode)
#define DeleteStackNode(self) DeletePrimitive((Primitive*)(self))

#define StackNode_getNext(self) ((self)->fBase.fOne.uStackNode)
#define StackNode_setNext(self, next) ((self)->fBase.fOne.uStackNode = (next))
#define StackNode_getData(self) ((self)->fBase.fTwo.uPrimitive)
#define StackNode_setData(self, data) ((self)->fBase.fTwo.uPrimitive = (Primitive*)(data))


/*
 *
 * List and List Node
 *
 */
struct TList{
  Primitive fBase;
};

List* NewList(void);
void DeleteList(List* self);
#define List_getTopNode(self) ((self)->fBase.fOne.uListNode)
#define List_setTopNode(self, node) ((self)->fBase.fOne.uListNode = (node))
#define List_getTopNode(self) ((self)->fBase.fOne.uListNode)
#define List_setTopNode(self, node) ((self)->fBase.fOne.uListNode = (node))
void List_push(List* stack, Primitive* item);
Primitive* List_pop(List* stack);
void List_print(List* stack);

struct TListNode {
  Primitive fBase;
};

#define NewListNode() NewPrimitive(ListNode)
#define DeleteListNode(self) DeletePrimitive((Primitive*)(self))

#define ListNode_getNext(self) ((self)->fBase.fOne.uListNode)
#define ListNode_setNext(self, next) ((self)->fBase.fOne.uListNode = (next))
#define ListNode_getData(self) ((self)->fBase.fTwo.uPrimitive)
#define ListNode_setData(self, data) ((self)->fBase.fTwo.uPrimitive = (Primitive*)(data))







#endif /* PRIMITIVE_H */
