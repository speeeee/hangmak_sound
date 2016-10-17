#ifndef GLOB_UTIL_H
#define GLOB_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <glad/glad.h>

#define NO_COLLISION    0
#define RIGID_COLLISION 1

typedef struct GState GState;
typedef struct Vec3 Vec3;
typedef struct Vec2 Vec2;
typedef struct Matrix Matrix;

typedef void (*StateChange)(GState *);
typedef void (*CollEffect)(GState *, Vec3 /* normal */);

typedef struct { GLfloat cxz; GLfloat cyz; } Camera;
typedef struct { GLfloat x; GLfloat y; GLfloat z; GLfloat tht; GLfloat phi; } KState;

typedef struct Queue { struct Queue *n; } Queue;
typedef struct { int t; int act; } Instr;

typedef struct { void *a; int sz; } Array; // by context is the type known.
                                           // the type will be written before upon initialization.

GLfloat deg_rad(GLfloat);
GLfloat deg_rad(GLfloat);
GLfloat cos_rad(GLfloat);
GLfloat sin_rad(GLfloat);

struct Vec3 { GLfloat x; GLfloat y; GLfloat z; };
struct Vec2 { GLfloat x; GLfloat y; };
struct Matrix { /* GLfloat */ Array pts; int x; int y; };
Matrix matrix(Array, int, int);
typedef struct { GLfloat x; GLfloat y; GLfloat z; Vec3 vel; Vec3 acc; } Player;
//typedef struct { Vec3 pos; Vec3 normal; } Plane;
typedef struct { /* Vec3 */ Array pts; int type; Vec3 normal; } Plane;
// see 'stage_util.h' for macros corresponding to surface type.
// assumes only X and Z planes are used.
// TODO: make array that maps Plane type to CollEffect and an array Surface type to StateChange.
typedef struct { /* Vec3 */ Array pts; int surf_type; int effect_type; } Surface;
// see 'stage_util.h' for macros corresponding to effect_type and surf_type.
typedef struct { /* Plane */ Array pln; int par; } PStage;
typedef struct { /* Surface */ Array sfs; int par; } Stage;
struct GState { Player pl; Camera ca; KState lk; int t;
                Instr *evs; int esz; GLfloat gravity; };

Vec3 v3(GLfloat, GLfloat, GLfloat);
Vec2 v2(GLfloat, GLfloat);
// warning: allocation.
Array Vec3_arr(int, ...);
Array Plane_arr(int, ...);
Array Surface_arr(int, ...);
Array GLdouble_arr(int, ...);
Array Vec2_arr(int, ...);

Vec3 cross(Vec3, Vec3);
//Vec3 norm(Vec3);

Vec3 inv(Vec3);

Vec3 norm(Vec3);
Vec3 rotate_vec(GLfloat, Vec3, Vec3);

GLfloat dot(Vec3, Vec3);
GLfloat vec_len(Vec3);
GLfloat angle(Vec3, Vec3);

void vert3(Vec3);

#endif
