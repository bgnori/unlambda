#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glib.h>

#include "object.h"


#define MAXOBJECTS 1000

int runnable(World* world){
  Object *operand, *operator, *q;
  Object* stack;
  Memory* m;

  m = World_getMemory(world);
  stack = Memory_getNth(m, 4).fTarget; /*FIXME */

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


Object* quote(World* world){
  Object* r;
  r = World_newObject(world, sizeof(Object));
  r->fProc = NULL;
  r->fOne.uObject = NULL;
  r->fName = "quote";
  return r;
}


Object* _print(World* world, Object* self, Object* other){
  printf("%c", self->fChar);
  return other;
}

Object* print(World* world, char x){
  Object* r;
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_print;
  r->fChar = x;
  r->fName = "print";
  return r;
}


Object* _identity(World* world, Object* self, Object* other){
  return other;
}

Object* identity(World* world){
  Object* r;
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_identity;
  r->fOne.uObject = NULL;
  r->fName = "identity";
  return r;
}


Object* _k1(World* world, Object* self, Object* other){
  return self->fOne.uObject;
}


Object* _constant_function(World* world, Object* self, Object* other){
  Object* k1;
  k1 = World_newObject(world, sizeof(Object));
  k1->fProc = _k1;
  k1->fName = "k1";
  k1->fOne.uObject = other;
  return k1;
}

Object* constant_function(World* world){
  /*
   *  k manufactures constant functions: the result of `kx is a function which,
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_constant_function;
  r->fName = "constant_function";
  return r;
}


Object* _s2(World* world, Object* self, Object* other){
  /* self == s2, other == z */
  Object* x; Object* y; Object*z;
  Object* xz; Object* yz;
  Object* r;
  z = other;
  y = self->fOne.uObject;
  x = self->fTwo.uObject->fOne.uObject;
  xz = World_call(world, x, z);
  yz = World_call(world, y, z);
  r = World_call(world, xz, yz);

  return r;
}


Object* _s1(World* world, Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = World_newObject(world, sizeof(Object));
  s2->fProc = &_s2;
  s2->fOne.uObject = other;
  s2->fTwo.uObject = self; /* may be it is better idea to reference x, instead of s1 */
  s2->fName = "s2";
  return s2;
}


Object* _generalized_evaluation(World* world, Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = World_newObject(world, sizeof(Object));
  s1->fProc = &_s1;
  s1->fOne.uObject = other;
  s1->fName = "s1";
  return s1;
}

Object* generalized_evaluation(World* world){
  /*
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = World_newObject(world, sizeof(Object));
  r->fProc = &_generalized_evaluation;
  r->fOne.uObject = NULL;
  r->fName = "generalized_evaluation";
  return r;
}


void run_once(World* world){
  Object *operand, *operator, *q, *r;

  operand = pop(world);
  operator = pop(world);
  q = pop(world);
  r = World_call(world, operator, operand);
  push(world, r);
}



void  World_eval(World* world, char* xs){
  char x;

  while (*xs){
    x = *xs;

    switch(x){
      case '`':
        push(world, quote(world));
        break;

      case '.':
        xs++;
        push(world, (print(world, *xs)));
        break;

      case 'r':
        push(world, (print(world, '\n')));
        break;

      case 'i':
        push(world, identity(world));
        break;

      case 'k':
        push(world, constant_function(world));
        break;

      case 's':
        push(world, generalized_evaluation(world));
        break;

      default:
        break;
    }
    while (runnable(world)){
      if(World_getDebug(world))
        print_stack(world);
      run_once(world);
      mark(world);
      sweep(world);
      if(World_getDebug(world))
        mem_stat(world);
    }
    xs++;
  }
  while (runnable(world)){
    if(World_getDebug(world))
      print_stack(world);
    run_once(world);
    mark(world);
    sweep(world);
    if(World_getDebug(world))
      mem_stat(world);
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

  printf("%s\n", fib);
  //eval(hw);
  World_unlambda(world, fib);
  DeleteWorld(world);

  return 0;
}
