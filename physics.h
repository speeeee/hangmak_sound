#include <stdlib.h>
#include <math.h>
#include "glob_structs.h"

#include <glad/glad.h>

#define FLOAT_MIN (-65535)
#define EPSILON (0.01)
//#define INFINITY (1./0)

// all is done using the y-axis.

void rigid_collision(GState *, Vec3);
void rigid_collision_simp(GState *, Vec3);

// warning: free result.
Vec3 *project_wrt_normal(/* Vec3 */ Array, Vec3);

#define vec2_minmax_y(A,B,MINMAX) MINMAX(A.y, B.y)?A:B;
#define min(A,B) (A)>(B)?B:A
#define max(A,B) (A)<(B)?B:A

// TODO: make cleaner the code.
int ray_intersects(Vec2, Vec3, Vec3);

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

int within_bounds(Vec3, Surface);
int test_collision_below(Vec3, Surface);

