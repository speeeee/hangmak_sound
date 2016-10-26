#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <glad/glad.h>

// -- see header ------------- //

#define D (GLdouble)

typedef struct GState GState;
typedef struct Vec3 Vec3;
typedef struct Vec2 Vec2;
typedef struct Matrix Matrix;

typedef void (*StateChange)(GState *);
typedef void (*CollEffect)(GState *, Vec3 /* normal */);
typedef GLfloat (*FuncXZ)(GLfloat x, GLfloat z);

typedef struct { GLfloat cxz; GLfloat cyz; } Camera;
typedef struct { GLfloat x; GLfloat y; GLfloat z; GLfloat tht; GLfloat phi; } KState;

typedef struct Queue { struct Queue *n; } Queue;
typedef struct { int t; int act; } Instr;

typedef struct { void *a; int sz; } Array; // by context is the type known.
                                            // the type will be written before upon initialization.

GLfloat signum(GLfloat a) { (a>0.)-(a<0.); }

GLfloat deg_rad(GLfloat x) { return x/180.*M_PI; }
GLfloat cos_rad(GLfloat x) { return cos(deg_rad(x)); }
GLfloat sin_rad(GLfloat x) { return sin(deg_rad(x)); }

struct Vec3 { GLfloat x; GLfloat y; GLfloat z; };
struct Vec2 { GLfloat x; GLfloat y; };
struct Matrix { /* GLfloat */ Array pts; int x; int y; };
Matrix matrix(Array a, int x, int y) { return (Matrix) { a, x, y }; }
typedef struct { GLfloat x; GLfloat y; GLfloat z; Vec3 vel; Vec3 acc; } Player;
//typedef struct { Vec3 pos; Vec3 normal; } Plane;
typedef struct { /* Vec3 */ Array pts; int type; Vec3 normal; } Plane;
// TriPlane.pf is expected to be a plane.
typedef struct { Vec3 a; Vec3 b; Vec3 c; int effect_type; Vec3 normal; } TriPlane;
typedef struct { Vec3 bl; Vec3 tr; FuncXZ fun; int effect_type; } Surface;
// let `bl' be the origin (0,0) wrt fun.
// see 'stage_util.h' for macros corresponding to surface type.
// assumes only X and Z planes are used.
// TODO: make array that maps Plane type to CollEffect and an array Surface type to StateChange.
//typedef struct { /* Vec3 */ Array pts; int surf_type; int effect_type; } Surface;
// see 'stage_util.h' for macros corresponding to effect_type and surf_type.
typedef struct { /* Surface */ Array sfs; int par; } Stage;
struct GState { Player pl; Camera ca; KState lk; int t;
                Instr *evs; int esz; GLfloat gravity; };

// ---------------------------- //

Vec3 v3(GLfloat x, GLfloat y, GLfloat z) { return (Vec3) { x, y, z }; }
Vec2 v2(GLfloat x, GLfloat y) { return (Vec2) { x, y }; }
#define ARRAY_FOR_TYPE(TYPE) Array TYPE ## _arr(int sz, ...) { Array n; n.sz = sz; \
  TYPE *a = malloc(n.sz*sizeof(TYPE)); va_list vl; va_start(vl,sz); \
  for(int i=0;i<sz;i++) { a[i] = va_arg(vl,TYPE); } n.a = (void *)a; va_end(vl); return n; }
ARRAY_FOR_TYPE(Vec3)
ARRAY_FOR_TYPE(Plane)
ARRAY_FOR_TYPE(Surface)
ARRAY_FOR_TYPE(GLdouble)
ARRAY_FOR_TYPE(Vec2)

Vec3 vec_add(Vec3 a, Vec3 b) { return v3(a.x+b.x,a.y+b.y,a.z+b.z); }
Vec3 vec_mul(Vec3 a, Vec3 b) { return v3(a.x*b.x,a.y*b.y,a.z*b.z); }
Vec3 scalar_mul(GLfloat scalar, Vec3 a) { return v3(scalar*a.x,scalar*a.y,scalar*a.z); }

Vec3 cross(Vec3 a, Vec3 b) {
  return (Vec3) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }

Vec3 inv(Vec3 a) { return v3(-a.x,-a.y,-a.z); }
GLfloat vec_len(Vec3 a) { return pow(a.x*a.x+a.y*a.y+a.z*a.z,0.5); }

Vec3 norm(Vec3 a) { GLfloat len = vec_len(a);
  return v3(a.x/len,a.y/len,a.z/len); }

// assume correctly sized for multiplication.
Vec3 transform3(Matrix a, Vec3 b) { GLfloat *arr = (GLfloat *)a.pts.a;
  return v3(arr[0]*b.x+arr[1]*b.y+arr[2]*b.z, arr[3]*b.x+arr[4]*b.y+arr[5]*b.z
           ,arr[6]*b.x+arr[7]*b.y+arr[8]*b.z); }
Vec3 rotate_vec(GLfloat tht, Vec3 subj, Vec3 axis) {
  GLfloat ty = sin_rad(tht); GLfloat tx = cos_rad(tht);
  // TODO: free this.
  Matrix rotation_matrix = matrix(GLdouble_arr(9
   ,D axis.x*axis.x*(1-tx)+tx, D axis.y*axis.x*(1-tx)-axis.z*ty, D axis.z*axis.x*(1-tx)+axis.y*ty
   ,D axis.x*axis.y*(1-tx)+axis.z*ty, D axis.y*axis.y*(1-tx)+tx, D axis.z*axis.y*(1-tx)-axis.x*ty
   ,D axis.x*axis.z*(1-tx)-axis.y*ty, D axis.y*axis.z*(1-tx)+axis.x*ty, D axis.z*axis.z*(1-tx)+tx )
  ,3,3);
  free(rotation_matrix.pts.a);
  return transform3(rotation_matrix,subj); }

GLfloat dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
GLfloat dot2(Vec2 a, Vec2 b) { return a.x*b.x+a.y*b.y; }
#define dot_aa(AA,AB,A,B)  dot2(v2(A.AA,A.AB),v2(B.AA,B.AB))

GLfloat angle(Vec3 a, Vec3 b) { return acos(dot(a,b)/(vec_len(a)*vec_len(b))); }

void vert3(Vec3 a) { glVertex3f(a.x,a.y,a.z); }
