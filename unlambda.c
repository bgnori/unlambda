#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glib.h>

#include "object.h"
#include "unlambda.h"

#define MAXOBJECTS 1000



typedef struct TUnlambdaEval UnlambdaEval;
struct TUnlambdaEval {
  Object fBase;
};

Object* Unlambda_call(Object* self, Object* other);

#define UnlambdaEval_getMemory(self) ((Memory*)((self)->fBase.fOne.uObject))
#define UnlambdaEval_getStack(self) ((Stack*)((self)->fBase.fTwo.uObject))

UnlambdaEval* NewUnlambdaEval(World* world){
  UnlambdaEval* ue;
  ue = NewObject(UnlambdaEval);

  ue->fBase.fOne.uObject = (Object*)World_getMemory(world);
  ue->fBase.fTwo.uObject = (Object*)NewStack();
  return ue;
}

void DeleteUnlambdaEval(UnlambdaEval* self){
  Stack* stack;
  Object* obj;
  Memory* memory;

  stack = UnlambdaEval_getStack(self);
  obj = Stack_pop(stack);
  while(obj){
    DeleteObject(obj);
    obj = Stack_pop(stack);
  }

  memory = UnlambdaEval_getMemory(self);
  Memory_mark(memory, stack);
  Memory_sweep(memory);
}



int runnable(void){
  Object *operand, *operator, *q;
  Stack* stack;
  Memory* m;

  m = World_getMemory(getWorld());
  stack = (Stack*)(Memory_NthEntry(m, 4)->fTarget); /*FIXME */

  operand = Stack_pop(stack);
  if(!operand){
    return false;
  }
  if(!operand->fProc){
    Stack_push(stack, operand);
    return false;
  }
  operator = Stack_pop(stack);
  if(!operator){
    Stack_push(stack, operand);
    return false;
  }
  if(!operator->fProc){
    Stack_push(stack, operator);
    Stack_push(stack, operand);
    return false;
  }

  q = Stack_pop(stack);
    Stack_push(stack, operator);
    Stack_push(stack, operand);
  if(!q){
    return false;
  }
  if(q->fProc){
    Stack_push(stack, operator);
    Stack_push(stack, operand);
    Stack_push(stack, q);
    return false;
  }
  return true;
}


Object* quote(){
  Object* r;
  r = NewObject(Object);
  r->fProc = NULL;
  r->fOne.uObject = NULL;
  r->fName = "quote";
  return r;
}


Object* _print(Object* self, Object* other){
  printf("%c", self->fOne.uChar);
  return other;
}

Object* print(char x){
  Object* r;
  r = NewObject(Object);
  r->fProc = &_print;
  r->fOne.uChar = x;
  r->fName = "print";
  return r;
}


Object* _identity(Object* self, Object* other){
  return other;
}

Object* identity(void){
  Object* r;
  r = NewObject(Object);
  r->fProc = &_identity;
  r->fOne.uObject = NULL;
  r->fName = "identity";
  return r;
}


Object* _k1(Object* self, Object* other){
  return self->fOne.uObject;
}


Object* _constant_function(Object* self, Object* other){
  Object* k1;
  k1 = NewObject(Object);
  k1->fProc = _k1;
  k1->fName = "k1";
  k1->fOne.uObject = other;
  return k1;
}

Object* constant_function(void){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = NewObject(Object);
  r->fProc = &_constant_function;
  r->fName = "constant_function";
  return r;
}


Object* _s2(Object* self, Object* other){
  /* self == s2, other == z */
  Object* x; Object* y; Object*z;
  Object* xz; Object* yz;
  Object* r;
  z = other;
  y = self->fOne.uObject;
  x = self->fTwo.uObject->fOne.uObject;
  xz = Unlambda_call(x, z);
  yz = Unlambda_call(y, z);
  r = Unlambda_call(xz, yz);

  return r;
}


Object* _s1(Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = NewObject(Object);
  s2->fProc = &_s2;
  s2->fOne.uObject = other;
  s2->fTwo.uObject = self; /* may be it is better idea to reference x, instead of s1 */
  s2->fName = "s2";
  return s2;
}


Object* _generalized_evaluation(Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = NewObject(Object);
  s1->fProc = &_s1;
  s1->fOne.uObject = other;
  s1->fName = "s1";
  return s1;
}

Object* generalized_evaluation(void){
  /*
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = NewObject(Object);
  r->fProc = &_generalized_evaluation;
  r->fOne.uObject = NULL;
  r->fName = "generalized_evaluation";
  return r;
}


void UnlambdaEval_runOnce(UnlambdaEval* self){
  Object *operand, *operator, *q, *r;
  Stack* stack;

  stack = UnlambdaEval_getStack(self);

  operand = Stack_pop(stack);
  operator = Stack_pop(stack);
  q = Stack_pop(stack);
  r = Unlambda_call(operator, operand);
  Stack_push(stack, r);
}


Object* Unlambda_call(Object* self, Object* other){
  if(World_getDebug(getWorld())){
      Object_print(self, "operator");
      Object_print(other, "operand");
  }
  return self->fProc(self, other);
}




void UnlambdaEval_eval(UnlambdaEval* self, char* xs){
  char x;
  Stack* stack;
  Memory * memory;

  stack = UnlambdaEval_getStack(self);
  memory = UnlambdaEval_getMemory(self);

  while (*xs){
    x = *xs;

    switch(x){
      case '`':
        Stack_push(stack, quote());
        break;

      case '.':
        xs++;
        Stack_push(stack, (print(*xs)));
        break;

      case 'r':
        Stack_push(stack, (print('\n')));
        break;

      case 'i':
        Stack_push(stack, identity());
        break;

      case 'k':
        Stack_push(stack, constant_function());
        break;

      case 's':
        Stack_push(stack, generalized_evaluation());
        break;

      default:
        break;
    }
    while (runnable()){
      if(World_getDebug(getWorld()))
        Stack_print(stack);
      UnlambdaEval_runOnce(self);
      Memory_mark(memory, stack);
      Memory_sweep(memory);
      if(World_getDebug(getWorld()))
        Memory_stat(memory);
    }
    xs++;
  }
  while (runnable()){
    if(World_getDebug(getWorld()))
      Stack_print(stack);
    UnlambdaEval_runOnce(self);
    Memory_mark(memory, stack);
    Memory_sweep(memory);
    if(World_getDebug(getWorld()))
      Memory_stat(memory);
  }

  return;
}


gint main(gint argc, gchar* argv[]){
  char* hw = "`r```````````.H.e.l.l.o. .w.o.r.l.di";
  char* fib =  "```s``s``sii`ki\n"
"`k.*``s``s`ks\n"
"``s`k`s`ks``s``s`ks``s`k`s`kr``s`k`sikk\n"
"`k``s`ksk";
  /*
   * http://www.madore.org/~david/programs/unlambda/
   */
  World* world;
  UnlambdaEval* ue;
  GOptionContext *context;
  GError *error = NULL;

  context = g_option_context_new("This line is appeared after Uagese's command line");
  g_option_context_set_summary(context, "This line is appeared before options");
  g_option_context_set_description(context, "This line is appeared after options");
  if(!g_option_context_parse(context, &argc, &argv, &error)){
    g_printerr("option parsing failed: %s\n", error->message);
    return 1;
  }

  world = CreateWorld(MAXOBJECTS);
  setWorld(world);
  ue = NewUnlambdaEval(world);

  printf("%s\n", fib);
  //eval(hw);
  UnlambdaEval_eval(ue, fib);
  DeleteWorld(world);

  return 0;
}


