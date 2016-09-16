#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct { GLfloat x; GLfloat y; GLfloat z; } Pt;

GLfloat deg_rad(GLfloat);

void vert3(Pt);

void quad(Pt, Pt, Pt, Pt);

// draws rect at angle around axis.
// NOTE: this cannot be placed between a glBegin..glEnd block.  It will generate its own
//       block.
void quadp(Pt, Pt, Pt, Pt, Pt, GLfloat); 
  
