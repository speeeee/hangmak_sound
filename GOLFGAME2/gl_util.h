#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glob_structs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//typedef struct { GLfloat x; GLfloat y; GLfloat z; } Pt;
typedef Vec3 Pt;
Pt pt(GLfloat, GLfloat, GLfloat);

void vert3(Pt);

void quad(Pt, Pt, Pt, Pt);

// draws rect at angle around axis.
// NOTE: this cannot be placed between a glBegin..glEnd block.  It will generate its own
//       block.
void quadp(Pt, Pt, Pt, Pt, Pt, GLfloat); 
void cube(Pt, GLfloat);

Pt cross(Pt, Pt);

#endif
