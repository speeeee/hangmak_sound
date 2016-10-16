#include <stdlib.h>
#include "glob_structs.h"

#include <glad/glad.h>

void rigid_collision(GState *g, Vec3 normal) { ivel = inv(g->pl.vel);
  GLfloat tht = angle(ivel,normal);
  Vec3 pla = cross(ivel,normal); // vector normal to plane with angle of vel wrt normal.
  // TODO: reflect velocity of ball wrt normal.  rotate wrt plane normal.
  // TODO: reflect position of ball wrt normal.  rotate wrt plane normal.
  g->pl.vel = rotate_vec(tht,ivel,norm(pla)); }

///* Vec3 */ Array project_wrt_normal(/* Vec3 */ Array a, Vec3 normal) {
/*  
int within_bounds(Vec3 a, Plane p) { // if the point is within the space that extends outward from
                                     // the plane, then it is within the bounds of it.
  // TODO: project wrt normal vector and test 2D.
int test_collision(GState *g, Vec3 hyp, Plane p) { // hyp is the hypothetical position after
                                                   // calculations with surfaces and without
                                                   // planes.
  // TODO: project wrt vector parallel to plane and test 2D.*/
