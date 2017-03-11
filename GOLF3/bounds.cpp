#include <cmath>

#include "world.hpp"

// contains various operations for bounds-checking.  specifically, bounds-checking for a point
//   in a triangle are included, as well as for determining if two points are on opposing sides of
//   a line.

// checks if a point (x,y) lies within a triangle ((x1,z1),(x2,z2),(x3,z3))
//   through barycentric coordinates.
//   given the system:
//     x = A(x1)+B(x2)+C(x3), y = A(z1)+B(z2)+C(z3), A+B+C = 1
//   for the point to lie within the triangle,
//   the following must be true: 0 <= A <= 1, 0 <= B <= 1, 0 <= C <= 1.

// expects a triangle and a 2D vector. 
int in_triangle(Vec2 e, Triangle b) { Vec2 et = vsub2(e,proj_xz(b.pos));
  float de = ((b.b.z-b.c.z)*(b.a.x-b.c.x)+(b.c.x-b.b.x)*(b.a.z-b.c.z));
  float a = ((b.b.z-b.c.z)*(et.x-b.c.x)+(b.c.x-b.b.x)*(et.z-b.c.z))/de;
  float r = ((b.c.z-b.a.z)*(et.x-b.c.x)+(b.a.x-b.c.x)*(et.z-b.c.z))/de;
  return 0 <= a && a <= 1 && 0 <= r && r <= 1 && 0 <= (1-a-r) && (1-a-r) <= 1; }

// TODO: for now, do simple bounds-checking by projecting triangle onto XZ-axis.
//     : this however makes near-vertical triangles easy to pass; this is a temporary solution.
//     : the predicted next state should be used for this.

// finds distance between point and triangle (or the plane parallel to triangle) by
//   Hesse normal form. (distance can be negative)
float dist_pt_plane(Vec3 pt, Triangle a) { return dot(vsub3(pt,a.pos),a.norm); }
float dist_ball_plane(Projectile b, Triangle a) {
  return dist_pt_plane(vadd3(vneg(vsmul(b.rad,unit(a.norm))),b.pos),a); }

int sign(float a) { return (a>0)-(a<0); }

// tests if two 3D-vector states s1 and s2 are on opposing sides of plane parallel to triangle a.
// TODO: add buffer zone to account for floating-point error. (the point could be calculated wrong
//     : when on the plane at a certain state.
int pl_side(Vec3 s1, Vec3 s2, Triangle a) {
  int r1 = sign(dist_pt_plane(s1,a)); int r2 = sign(dist_pt_plane(s2,a));
  return !r1||!r2||!(r1+r2); }
int pl_side_ball(Projectile p1, Projectile p2, Triangle a) {
  int r1 = sign(dist_ball_plane(p1,a)); int r2 = sign(dist_ball_plane(p2,a));
  return !r1||!r2||!(r1+r2); }

// tests if distance is negative.
int pl_side_ball_low(Projectile p1, Projectile p2, Triangle a) {
  int r1 = sign(dist_ball_plane(p1,a)); int r2 = sign(dist_ball_plane(p2,a));
  return !r1||!r2||(r1+r2)<1; }
