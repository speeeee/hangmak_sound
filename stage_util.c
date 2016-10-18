// TODO: make array that maps Plane type to CollEffect and an array Surface type to StateChange.
#include <stdlib.h>
#include <glad/glad.h>
#include <math.h>
#include "glob_structs.h"

Plane plane(Array pts, int t, Vec3 normal) { return (Plane) { pts, t, normal }; }
// renders 2D surface at normal p.normal.
/*void render_plane(Plane p) { glPushMatrix();
  GLfloat tht = acos(dot(v3(0,1,0),p.normal)/len(p.normal));
  Vec3 cr = cross(v3(0,1,0),p.normal);
  glRotatef(tht,cr.x,cr.y,cr.z); // possibility that this must be negative.
  glBegin(GL_POLYGON);
  for(int i=0;i<p.pts.sz;i++) { vert3(((Vec3 *)p.pts.a)[i]); } glEnd();
  glPopMatrix(); }*/
void render_plane(Plane p) { glBegin(GL_POLYGON);
  for(int i=0;i<p.pts.sz;i++) { vert3(((Vec3 *)p.pts.a)[i]); } glEnd(); }
//DONE: surface and plane rendering functions.
// TODO: make this function nicer.
void render_surface(Surface s, GLfloat step) { // step is the interval between y-tests.
  glBegin(GL_QUADS);
  //vert3(v3(s.br.x,s.fun(s.br.x,s.br.z)-s.br.y,s.br.z));
  //vert3(v3(s.br.x+step,s.fun(s.br.x+step,s.br.z)-s.br.y,s.br.z));
  //vert3(v3(s.br.x+step,s.fun(s.br.x+step,s.br.z+step)-s.br.y,s.br.z+step));
  //vert3(v3(s.br.x,s.fun(s.br.x,s.br.z+step)-s.br.y,s.br.z+step));
  for(int q=0;q<(int)((s.tr.z-s.bl.z)/step);q++) {
    for(int i=0;i<(int)((s.tr.x-s.bl.x)/step);i++) {
      vert3(v3(s.bl.x+step*i,s.fun(s.bl.x+step*i,s.bl.z+step*q),s.bl.z+step*q));
      vert3(v3(s.bl.x+step*(i+1),s.fun(s.bl.x+step*(i+1),s.bl.z+step*q),s.bl.z+step*q));
      vert3(v3(s.bl.x+step*(i+1),s.fun(s.bl.x+step*(i+1),s.bl.z+step*(q+1)),s.bl.z+step*(q+1)));
      vert3(v3(s.bl.x+step*i,s.fun(s.bl.x+step*i,s.bl.z+step*(q+1)),s.bl.z+step*(q+1))); } }
  glEnd(); }
