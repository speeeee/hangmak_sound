#include <stdio.h>
#include <stdlib.h>

#include <sndfile.h>

typedef struct { int *data; int sz; } Snd;
typedef struct { int a; int nothing; } MaybeI;
MaybeI maybeI(int a, int nothing) { return (MaybeI) { a, nothing }; }

typedef int (*IFun)(int);

// variable __IND defined for every callback call.  Used only for bounds-checking.

// the callback stops once Nothing has been found during the call.

// Time -> Double -> Snd -> MaybeI Int
#define pitch(T,D,S) \
  return (__IND = (int)((double)(T)+(D)))>(S).sz? \
    maybeI(0, 0):maybeI((S)[__IND], 1)

MaybeI fmap(MaybeI a, MaybeI b, IFun f) {
  if(!(a.nothing&&b.nothing)) { return maybeI(0,0); } return maybeI((f)(a.a,b.a),1); }
