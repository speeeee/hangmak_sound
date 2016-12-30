#include "world.hpp"
#include <cmath>

Vec3 v3(float x, float y, float z) { return (Vec3) { x, y, z }; }
Vec2 v2(float x, float z) { return (Vec2) { x, z }; }
Triangle triangle(Vec3 pos, Vec2 a, Vec2 b, Vec2 c, Vec3 norm) {
  return (Triangle) { pos, a, b, c, norm }; }

Vec2 centroid(Triangle a) { return v2((a.a.x+a.b.x+a.c.x)/3.,(a.a.z+a.b.z+a.c.z)/3.); }

Vec2 vsub2(Vec2 a, Vec2 b) { return v2(a.x-b.x,a.z-b.z); }
Vec3 vsub3(Vec3 a, Vec3 b) { return v3(a.x-b.x,a.y-b.y,a.z-b.z); }

Triangle t_centroid(Triangle a) { Vec2 c = centroid(a);
  return triangle(a.pos,vsub2(a.a,c),vsub2(a.b,c),vsub2(a.c,c),a.norm); }

Vec3 unit(Vec3 a) { float u = pow(pow(a.x,2)+pow(a.y,2)+pow(a.z,2),0.5);
  return (Vec3) { a.x/u, a.y/u, a.z/u }; }
float dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
float len(Vec3 a) { return pow(pow(a.x,2.)+pow(a.y,2.)+pow(a.z,2.),0.5); }
float angle(Vec3 a, Vec3 b) { float la = len(a); float lb = len(b);
  return acos(dot(a,b)/(la*lb)); }

Vec3 cross(Vec3 a, Vec3 b) {
  return (Vec3) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }

