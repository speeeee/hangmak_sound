#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct { GLfloat x; GLfloat y; GLfloat z; } Pt;

GLfloat deg_rad(GLfloat x) { return x/180.*M_PI; }

void vert3(Pt a) { glVertex3f(a.x,a.y,a.z); }

void quad(Pt a, Pt b, Pt c, Pt d) {
  vert3(a); vert3(b); vert3(c); vert3(c); }

// draws rect at angle around axis.
// NOTE: this cannot be placed between a glBegin..glEnd block.  It will generate its own
//       block.
void quadp(Pt a, Pt b, Pt c, Pt d, Pt axis, GLfloat tht) { glBegin(GL_QUADS);
  glPushMatrix(); glRotatef(tht,axis.x,axis.y,axis.z); quad(a,b,c,d); glPopMatrix();
  glEnd(); } 
  
