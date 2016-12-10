#include <stdlib.h>
#include <math.h>
#include "glob_structs.h"

#include <glad/glad.h>

#define FLOAT_MIN (-65535)
#define EPSILON (0.01)

#define DEGRADE_FACTOR (0.8) // 0.9 degrade is strange.
#define AUGMENT (1.5)
//#define INFINITY (1./0)

/*Vec3 next_position(GState g) {
  // change acceleration and velocity
  Vec3 na = v3(0.,g.gravity,0.); Vec3 nv = vec_add(na,g.pl.vel); 
  // change position
  return v3(g.pl.x+g.pl.vel.x, g.pl.y+g.pl.vel.y, g.pl.z+g.pl.vel.z); }*/
Vec3 next_position(GState g) {
  // change acceleration and velocity
  Vec3 na = v3(0.,g.gravity,0.); Vec3 nv = vec_add(na,g.pl.vel); 
  // change position
  return v3(g.pl.x+nv.x, g.pl.y+nv.y, g.pl.z+nv.z); }

// all is done using wrt the y-axis.

void rigid_collision(GState *g, FuncXZ fun, GradXZ d_fun) {
  // get vector that is the gradient at the point of f(x,z) with y as -1.
  //   this is the desired tangent vector.
  Vec3 nn = scalar_mul(-1,norm(with_y(d_fun(g->pl.x,g->pl.z))));
  // get angle (theta) between the vector and the y-axis.
  GLfloat tht = angle(nn,v3(0,-1,0)); //printf("%g\n",tht);
  // new vector where x and z are simply their partial derivatives and
  //   y is based off of the angle between the y-axis and the gradient vector.
  Vec3 newv = v3(nn.x,-2*nn.y*cos(tht),nn.z);
  //printf("<%g, %g, %g>\n", nn.x, nn.y, nn.z);
  // add to the old velocity |old velocity|*newv.
  GLfloat y_pos = fun(g->pl.x,g->pl.z);
  // set position to same as function but with small offset to reduce floating-point error.
  g->pl.y = y_pos+0.01*(GLfloat)signum(g->pl.y-y_pos);
  g->pl.vel = vec_add(g->pl.vel,scalar_mul(vec_len(g->pl.vel),newv)); }
void normal_collision(GState *g, FuncXZ fun, GradXZ d_fun) {
  Vec3 nn = norm(with_y(d_fun(g->pl.x,g->pl.z)));
  g->pl.vel = scalar_mul(vec_len(g->pl.vel),v3(-nn.x,nn.y,-nn.z)); }
void normal_degrade_collision(GState *g, FuncXZ fun, GradXZ d_fun) {
  Vec3 nn = norm(with_y(d_fun(g->pl.x,g->pl.z)));
  g->pl.vel = scalar_mul(DEGRADE_FACTOR*vec_len(g->pl.vel),v3(-nn.x,nn.y,-nn.z));
  //g->pl.y += g->pl.vel.y;
  printf("%g, %g, %g, (%g)\n",nn.y, fun(g->pl.x,g->pl.z), g->pl.y, g->pl.vel.y);
  g->pl.vel.y -= 0.005*(1-nn.y); }
void normal2_collision(GState *g, FuncXZ fun, GradXZ d_fun) {
  Vec3 nn = norm(with_y(d_fun(g->pl.x,g->pl.z)));
  GLfloat rel = -dot(v3(-nn.x,nn.y,-nn.z),norm(g->pl.vel)); printf("%g\n",rel);
  g->pl.vel = vec_add(g->pl.vel,scalar_mul((1+rel)*vec_len(g->pl.vel),v3(-nn.x,nn.y,-nn.z))); }

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
int test_collision_below(Vec3 a, Vec3 a_next, Surface s) {
  //if(a.y>=s.fun(a.x,a.z)-0.01) { printf("NOW: %g, NEXT: %g, Y: %g\n",a.y,a_next.y,s.fun(a.x,a.z)); }
  return within_bounds(a_next,s)
       &&((a.y<=s.fun(a.x,a.z)&&a_next.y>=s.fun(a_next.x,a_next.z))
        ||(a.y>=s.fun(a.x,a.z)&&a_next.y<=s.fun(a_next.x,a_next.z))); }
