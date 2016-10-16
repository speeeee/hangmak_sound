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
//TODO: surface and plane rendering functions. 
