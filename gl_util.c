#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct { GLfloat x; GLfloat y; GLfloat z; } Player;
typedef Player Pt;

Pt pt(GLfloat x, GLfloat y, GLfloat z) { return (Pt) { x, y, z }; }

GLfloat deg_rad(GLfloat x) { return x/180.*M_PI; }

void vert3(Pt a) { glVertex3f(a.x,a.y,a.z); }

void quad(Pt a, Pt b, Pt c, Pt d) {
  vert3(a); vert3(b); vert3(c); vert3(d); }

// draws rect at angle around axis.
// NOTE: this cannot be placed between a glBegin..glEnd block.  It will generate its own
//       block.
void quadp(Pt a, Pt b, Pt c, Pt d, Pt axis, GLfloat tht) { glPushMatrix();
  glRotatef(tht,axis.x,axis.y,axis.z); glBegin(GL_QUADS); quad(a,b,c,d); glEnd();
  glPopMatrix(); } 

void cube(Pt a, GLfloat l) { glPushMatrix(); glTranslatef(-a.x-l/2,-a.y,-a.z-l/2);
  for(int i=0;i<4;i++) { quadp(pt(-l/2,0,-l/2),pt(l/2,0,-l/2),pt(l/2,l,-l/2),pt(-l/2,l,-l/2)
                              ,pt(0,1,0),-90.*i); } glBegin(GL_QUADS);
  quad(pt(-l/2,0,-l/2),pt(l/2,0,-l/2),pt(l/2,0,l/2),pt(-l/2,0,l/2));
  quad(pt(-l/2,l,-l/2),pt(l/2,l,-l/2),pt(l/2,l,l/2),pt(-l/2,l,l/2)); glEnd(); glPopMatrix(); }

Pt cross(Pt a, Pt b) {
  return (Player) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }
