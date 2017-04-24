#include "world.hpp"
#include <cstdio>

void p_vec3(Vec3 a) { printf("<%g,%g,%g>\n",a.x,a.y,a.z); }

Vec3 vsub3(Vec3 a, Vec3 b) { return Vec3(a.x-b.x,a.y-b.y,a.z-b.z); }
Vec3 vadd3(Vec3 a, Vec3 b) { return Vec3(a.x+b.x,a.y+b.y,a.z+b.z); }

Vec3 operator+(Vec3 a, Vec3 b) { return vadd3(a,b); }
Vec3 operator-(Vec3 a, Vec3 b) { return vsub3(a,b); }
bool operator==(Vec3 a, Vec3 b) { return a.x==b.x&&a.y==b.y&&a.z==b.z; }

Vec3 vcmul(Vec3 a, Vec3 b) { return Vec3(a.x*b.x,a.y*b.y,a.z*b.z); }
Vec3 vsmul(float a, Vec3 b) { return Vec3(a*b.x,a*b.y,a*b.z); }
Vec3 operator*(float a, Vec3 b) { return vsmul(a,b); }

Vec3 vneg(Vec3 a) { return Vec3(-a.x,-a.y,-a.z); }
