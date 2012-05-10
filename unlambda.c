#include <stdio.h>
#include <stdlib.h>


#define false 0
#define true 1


typedef struct TObject Object;

typedef Object*(*Func)(Object* self, Object* other);

struct TObject {
  Object* fPrev;
  int fRefCount;
  Func fProc;
  union {
    Object * uObject;
    char uChar;
  } fValue;
  Object * fObject2;
};


Object* call(Object* self, Object* other){
  
  return self->fProc(self, other);
}


Object* NewObject(){
  Object* r;
  r = (Object*)malloc(sizeof(Object));
  r->fRefCount = 1;
  return r;
}

void Ref(Object* self){
  self->fRefCount +=1;
}

void DeleteObject(Object* self){
  self->fRefCount -= 1;
  if(self->fRefCount == 0){
    free((void*)self);
  }
}


Object* gstack;

void push(Object* obj){
  obj->fPrev = gstack;
  gstack = obj;
}

Object* pop(void){
  Object* r;
  r = gstack;
  gstack = gstack->fPrev;
  return r;
}


int runnable(void){
  Object *operand, *operator, *q;
  
  operand = gstack;
  if(!operand){
    return false;
  }
  if(!operand->fProc){
    return false;
  }
  operator = operand->fPrev;

  if(!operator){
    return false;
  }
  if(!operator->fProc){
    return false;
  }
  q = operator->fPrev;
  if(!q){
    return false;
  }
  if(q->fProc){
    return false;
  }
  return true;
}



Object* quote(void){
  Object* r;
  r = NewObject();
  r->fProc = NULL;
  r->fValue.uObject = NULL;
  return r;
}


Object* _print(Object* self, Object* other){
  printf("%c", self->fValue.uChar);
  return other;
}

Object* print(char x){
  Object* r;
  r = NewObject();
  r->fProc = &_print;
  r->fValue.uChar = x;
  return r;
}


Object* _identity(Object* self, Object* other){
  return other;
}

Object* identity(void){
  Object* r;
  r = NewObject();
  r->fProc = &_identity;
  r->fValue.uObject = NULL;
  return r;
}


Object* _constant_function(Object* self, Object* other){
  Ref(other);
  return other;
}

Object* constant_function(){
  /*
   *  k manufactures constant functions: the result of `kx is a function which, 
   * when invoked, returns x. Thus the value of ``kxy is x for any x and y.
   */
  Object* r;
  r = NewObject();
  r->fProc = &_constant_function;
  return r;
}




Object* _s2(Object* self, Object* other){
  /* self == s2, other == z */
  Object* x; Object* y; Object*z;
  Object* xz; Object* yz;
  Object* r;
  z = other;
  y = self->fValue.uObject;
  x = self->fObject2->fValue.uObject;
  xz = call(x, z);
  yz = call(y, z);
  r = call(xz, yz);

  DeleteObject(xz);
  DeleteObject(yz);
  DeleteObject(x);
  DeleteObject(y);
  DeleteObject(self->fObject2);/* s1 */
  return r;
}


Object* _s1(Object* self, Object* other){
  /* self == s1, other == y */
  Object* s2;
  s2 = NewObject();
  s2->fProc = &_s2;
  s2->fValue.uObject = other;
  Ref(other);
  s2->fObject2 = self; /* may be it is better idea to reference x, instead of s1 */
  Ref(self);
  return s2;
}


Object* _generalized_evaluation(Object* self, Object* other){
  /* self == s, other == x */
  Object* s1;
  s1 = NewObject();
  s1->fProc = &_s1;
  s1->fValue.uObject = other;
  Ref(other);
  return s1;
}

Object* generalized_evaluation(void){
  /* 
   * s is a generalized evaluation operator. ```sxyz evaluates to ``xz`yz for any x, y, and z."""
   */
  Object* r;/* s */
  r = NewObject();
  r->fProc = &_generalized_evaluation;
  r->fValue.uObject = NULL; 
  return r;
}


void run_once(void){
  Object *operand, *operator, *q, *r;

  operand = pop();
  operator = pop();
  q = pop();
  r = call(operator, operand);
  Ref(r);
  push(r);
  DeleteObject(operand);
  DeleteObject(operator);
  DeleteObject(q);
}


int eval(char* xs){
  char x;

  while (*xs){
    x = *xs;

    switch(x){
      case '`':
        push(quote());
        break;

      case '.':
        xs++;
        push((print(*xs)));
        break;

      case 'r':
        push((print('\n')));
        break;

      case 'i':
        push(identity());
        break;

      case 'k':
        push(constant_function());
        break;

      case 's':
        push(generalized_evaluation());
        break;

      default:
        break;
    }
    while (runnable()){
      run_once();
    }
    xs++;
  }

  return 0;
}

int main(int args, char** argv){
  char* hw = "`r```````````.H.e.l.l.o. .w.o.r.l.di";

  eval(hw);
  //printf("%s\n", hw);
  return 0;
}

