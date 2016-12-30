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
int in_triangle(Vec2 a, Triangle b) {
  float de = ((b.b.z-b.c.z)*(b.a.x-b.c.x)+(b.c.x-b.b.x)*(b.a.z-b.c.z));
  float a = ((b.b.z-b.c.z)*(a.x-b.c.x)+(b.c.x-b.b.x)*(a.z-b.c.z))/de;
  float b = ((b.c.z-b.a.z)*(a.x-b.c.x)+(b.a.x-b.c.x)*(a.z-b.c.z))/de;
  return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= (1-a-b) && (1-a-b) <= 1; }

// TODO: for now, do simple bounds-checking by projecting triangle onto XZ-axis.
//     : this however makes near-vertical triangles easy to pass; this is a temporary solution.

// finds distance between point and triangle (or the plane parallel to triangle) by
//   Hesse normal form. (distance can be negative)
float dist_pt_plane(Vec3 pt, Triangle a) { return dot(vsub3(pt,a.pos),a.norm); }
