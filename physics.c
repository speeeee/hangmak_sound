#include <stdlib.h>
#include <math.h>
#include "glob_structs.h"

#include <glad/glad.h>

#define FLOAT_MIN (-65535)
#define EPSILON (0.01)
//#define INFINITY (1./0)

// all is done using the y-axis.

void rigid_collision(GState *g, Vec3 normal) { Vec3 ivel = inv(g->pl.vel);
  GLfloat tht = angle(ivel,normal);
  Vec3 pla = cross(ivel,normal); // vector normal to plane with angle of vel wrt normal.
  // DONE: reflect velocity of ball wrt normal.  rotate wrt plane normal.
  // DONE: reflect position of ball wrt normal.  rotate wrt plane normal.
  g->pl.vel = rotate_vec(tht,ivel,norm(pla)); }
// intentionally incorrect for testing.
void rigid_collision_simp(GState *g, Vec3 normal) {
  GLfloat l = vec_len(g->pl.vel); Vec3 nn = norm(normal);
  g->pl.vel = v3(l*nn.x,l*nn.y,l*nn.z); }

// warning: free result.
Vec3 *project_wrt_normal(/* Vec3 */ Array a, Vec3 axis) {
  Vec3 *opts = (Vec3 *)a.a; Vec3 *ns = malloc(a.sz*sizeof(Vec3));
  for(int i=0;i<a.sz;i++) { ns[i] = v3(axis.x*opts[i].x,axis.y*opts[i].y
                                      ,axis.z*opts[i].z); }
  return ns; }

#define vec2_minmax_y(A,B,MINMAX) MINMAX(A.y, B.y)?A:B;
#define min(A,B) (A)>(B)?B:A
#define max(A,B) (A)<(B)?B:A

// TODO: make cleaner the code.
int ray_intersects(Vec2 v, Vec3 pa, Vec3 pb) {
  Vec3 a = pa.y>pb.y?pb:pa; Vec3 b = pa.y>pb.y?pa:pb;
  if(v.y==a.z||v.y==b.z) { v.y += EPSILON; }
  if(v.y<a.z||v.y>b.z||v.x>max(a.x,b.x)) { return 0; }
  else if(v.x<min(a.x,b.x)) { return 1; }
  else { GLfloat vr, vb; if(a.x != b.x) { vr = (b.z-a.z)/(b.x-a.x); }
         else { vr = INFINITY; }
         if(a.x != v.x) { vb = (v.y-a.y)/(v.x-a.x); }
         else { vb = INFINITY; } return vb >= vr; } return 0; }

// TODO: also project onto YZ- and XY-axis as other tests.  it passes if any of these passes.
// project test test vector and plane onto XZ-axis
/*int within_bounds(Vec3 a, Plane p) { // if the point is within the space that extends outward from
                                     // the plane, then it is within the bounds of it.
  Vec3 *vs = project_wrt_normal(p.pts,v3(1,0,1)); int res = 0;
  for(int i=0;i<p.pts.sz;i++) { res += ray_intersects(v2(FLOAT_MIN,a.z),vs[i],vs[(i+1)%p.pts.sz]); }
  return res%2; }*/
//int test_collision(GState *g, Vec3 hyp, Plane p) { // hyp is the hypothetical position after
//                                                   // calculations with surfaces and without
//                                                   // planes.
// assumes infinite size plane
// pt can be any point from the Plane.
// done wrt y-axis
//int test_plane_collision(GState g, Vec3 hyp, Vec3 normal, Vec3 pt) {
//  GLfloat tht = angle(normal,v3(0,1,0)); GLfloat slope = tan(tht); // check

int within_bounds(Vec3 a, Surface s) { return a.x>=s.bl.x&&a.x<=s.tr.x
                                            &&a.z>=s.bl.z&&a.z<=s.tr.z; }
int test_collision_below(Vec3 a, Surface s) {
  return within_bounds(a,s)&&a.y<s.fun(a.x,a.z); }
