#include "peer.h"
#include "primitive.h"

Primitive* 
_XNewPrimitive(size)
  int size;
{
  Primitive* r;
  r = (Primitive*)peer_alloc(size);
  r->fOne.uPrimitive = NULL;
  r->fTwo.uPrimitive = NULL;
  return r;
}


void 
_XDeletePrimitive(self)
  Primitive* self;
{
  peer_free((void*)self);
}


Primitive* 
_NewPrimitive(size)
  int size;
{
  Primitive* r;
  r = _XNewPrimitive(size);
  onNewPrimitive(r);
  return r;
}


void 
_DeletePrimitive(self)
  Primitive* self;
{
  onDeletePrimitive(self);
  _XDeletePrimitive(self);
}


void
Primitive_print(self)
  Primitive* self;
{
  /* UGH! */
}


Stack* 
NewStack(void)
{
  Stack* r;
  r = NewPrimitive(Stack);
  Stack_setTopNode(r, NULL);
  return r;
}

void 
DeleteStack(self)
  Stack* self;
{
  while(Stack_getTopNode(self)){
    Stack_pop(self);
  }
}

void 
Stack_push(self, item)
  Stack* self;
  Primitive* item;
{
  BinaryNode* n;
  n = NewBinaryNode();
  BinaryNode_setNext(n, Stack_getTopNode(self));
  BinaryNode_setData(n, item);
  Stack_setTopNode(self, n);
}

Primitive* 
Stack_pop(self)
  Stack* self;
{
  BinaryNode* n;
  n = Stack_getTopNode(self);
  if(!n){
    return NULL;
  }
  Stack_setTopNode(self, BinaryNode_getNext(n));
  return BinaryNode_getData(n);
}

void 
Stack_print(self)
  Stack* self;
{
  BinaryNode* n;
  n = Stack_getTopNode(self);
  while(n){
    Primitive_print(BinaryNode_getData(n));
    n = BinaryNode_getNext(n);
  }
}


List* 
NewList(void)
{
  List* r;
  r = NewPrimitive(List);
  List_setHeadNode(r, NULL);
  List_setTailNode(r, NULL);
  return r;
}


void
DeleteList(self)
  List* self;
{
  BinaryNode* n;
  n = List_getHeadNode(self);
  while(n){
    List_pop(self);
  }
}


void 
List_append(self, item)
  List* self;
  Primitive* item;
{
  BinaryNode* r;
  r = NewBinaryNode();
  BinaryNode_setData(r, item);
  BinaryNode_setNext(r, List_getTailNode(self));
  List_setTailNode(self, r); 
}


Primitive* 
List_pop(self)
  List* self;
{
  BinaryNode* r;
  r = List_getHeadNode(self);
  List_setHeadNode(self, BinaryNode_getNext(r));
  return BinaryNode_getData(r);
}


void
List_print(self)
  List* self;
{
  BinaryNode* n;
  Primitive* p;
  n = List_getHeadNode(self);

  while(n){
    p = BinaryNode_getData(n);
    Primitive_print(p);
    n = BinaryNode_getNext(n);
  }
}


