#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <glad/glad.h>

// -- see header ------------- //

typedef struct GState GState;
typedef struct Vec3 Vec3;

typedef void (*StateChange)(GState *);
typedef void (*CollEffect)(GState *, Vec3 /* normal */);

typedef struct { GLfloat cxz; GLfloat cyz; } Camera;
typedef struct { GLfloat x; GLfloat y; GLfloat z; GLfloat tht; GLfloat phi; } KState;

typedef struct Queue { struct Queue *n; } Queue;
typedef struct { int t; int act; } Instr;

typedef struct { void *a; int sz; } Array; // by context is the type known.
                                            // the type will be written before upon initialization.

struct Vec3 { GLfloat x; GLfloat y; GLfloat z; };
typedef struct { GLfloat x; GLfloat y; GLfloat z; Vec3 vel; Vec3 acc; } Player;
//typedef struct { Vec3 pos; Vec3 normal; } Plane;
typedef struct { /* Vec3 */ Array pts; int type; Vec3 normal; } Plane;
// see 'stage_util.h' for macros corresponding to surface type.
// assumes only X and Z planes are used.
// TODO: make array that maps Plane type to CollEffect and an array Surface type to StateChange.
typedef struct { /* Vec3 */ Array pts; int surf_type; int effect_type; } Surface;
// see 'stage_util.h' for macros corresponding to effect_type and surf_type.
typedef struct { /* Surface */ Array sfs; int par; } Stage;
struct GState { Player pl; Camera ca; KState lk; int t;
                Instr *evs; int esz; GLfloat gravity; };

// ---------------------------- //

Vec3 v3(GLfloat x, GLfloat y, GLfloat z) { return (Vec3) { x, y, z }; }
#define ARRAY_FOR_TYPE(TYPE) Array TYPE ## _arr(int sz, ...) { Array n; n.sz = sz; \
  TYPE *a = malloc(n.sz*sizeof(TYPE *)); va_list vl; va_start(vl,sz); \
  for(int i=0;i<sz;i++) { a[i] = va_arg(vl,TYPE); } n.a = (void *)a; va_end(vl); return n; }
ARRAY_FOR_TYPE(Vec3)
ARRAY_FOR_TYPE(Plane)
ARRAY_FOR_TYPE(Surface)
