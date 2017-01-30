#include "world.hpp"
#include <cmath>

Vec3 v3(float x, float y, float z) { return (Vec3) { x, y, z }; }
Vec2 v2(float x, float z) { return (Vec2) { x, z }; }
Triangle triangle(Vec3 pos, Vec2 a, Vec2 b, Vec2 c, Vec3 norm) {
  return (Triangle) { pos, a, b, c, norm }; }

Vec2 centroid(Triangle a) { return v2((a.a.x+a.b.x+a.c.x)/3.,(a.a.z+a.b.z+a.c.z)/3.); }

Vec2 vsub2(Vec2 a, Vec2 b) { return v2(a.x-b.x,a.z-b.z); }
Vec3 vsub3(Vec3 a, Vec3 b) { return v3(a.x-b.x,a.y-b.y,a.z-b.z); }

Vec2 vadd2(Vec2 a, Vec2 b) { return v2(a.x+b.x,a.z+b.z); }
Vec3 vadd3(Vec3 a, Vec3 b) { return v3(a.x+b.x,a.y+b.y,a.z+b.z); }

Vec3 operator+(Vec3 a, Vec3 b) { return vadd3(a,b); }
Vec3 operator-(Vec3 a, Vec3 b) { return vsub3(a,b); }
bool operator==(Vec3 a, Vec3 b) { return a.x==b.x&&a.y==b.y&&a.z==b.z; }

Vec2 proj_xz(Vec3 a) { return v2(a.x,a.z); }

Vec3 vcmul(Vec3 a, Vec3 b) { return v3(a.x*b.x,a.y*b.y,a.z*b.z); }
Vec3 vsmul(float a, Vec3 b) { return v3(a*b.x,a*b.y,a*b.z); }

Vec3 vneg(Vec3 a) { return v3(-a.x,-a.y,-a.z); }

float dist(Vec2 a, Vec2 b) { return sqrt(pow(b.x-a.x,2.)+pow(b.z-a.z,2.)); }

Vec3 arr_to_vec(float *t) { return v3(t[0],t[1],t[2]); }
void asadd(Vec3 pos, float *t, int sz) { for(int i=0;i<sz;i+=3) {
  t[i] += pos.x; t[i+1] += pos.y; t[i+2] += pos.z; } }

Projectile projectile(Vec3 acc, Vec3 vel, Vec3 pos, float rad) {
  return (Projectile) { acc, vel, pos, rad }; }

Triangle t_centroid(Triangle a) { Vec2 c = centroid(a);
  return triangle(a.pos,vsub2(a.a,c),vsub2(a.b,c),vsub2(a.c,c),a.norm); }
Triangle center_right(Triangle a, Vec3 orig, Vec3 xv, Vec3 yv) {
  //Vec3 xt = vsub3(xv,orig); Vec3 yt = vsub3(yv,orig);
  Vec2 cent = centroid(a);
  a.pos = v3(orig.x+cent.x,len2(cent)*dot(unit(a.norm),v3(1,0,0)),orig.z+cent.z);
  return a; }

Vec3 unit(Vec3 a) { float u = pow(pow(a.x,2)+pow(a.y,2)+pow(a.z,2),0.5);
  return (Vec3) { a.x/u, a.y/u, a.z/u }; }
float dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
float len(Vec3 a) { return pow(pow(a.x,2.)+pow(a.y,2.)+pow(a.z,2.),0.5); }
float len2(Vec2 a) { return pow(pow(a.x,2.)+pow(a.z,2.),0.5); }
float angle(Vec3 a, Vec3 b) { float la = len(a); float lb = len(b);
  return acos(dot(a,b)/(la*lb)); }

Vec3 cross(Vec3 a, Vec3 b) {
  return (Vec3) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }

VAOdat vao_dat(int disp, int sz, int nsteps, GLuint vao) {
  return (VAOdat) { disp, sz, nsteps, vao }; }
Entity entity(Vec3 pos, std::vector<Triangle> t, std::vector<float> vpts, VAOdat vao, BoundsF bf
             ,CollisionF cf, GLuint shader_id) {
  return (Entity) { pos, t, vpts, vao, bf, cf, shader_id }; }
