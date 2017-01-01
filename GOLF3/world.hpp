#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

typedef struct { float x; float y; float z; } Vec3;
Vec3 v3(float,float,float);
Vec3 cross(Vec3, Vec3);
Vec3 unit(Vec3);
float dot(Vec3, Vec3);
float len(Vec3);
float angle(Vec3 a, Vec3 b);

typedef struct { float x; float z; } Vec2;
Vec2 v2(float,float);

Vec2 vsub2(Vec2, Vec2);
Vec3 vsub3(Vec3, Vec3);

Vec2 vadd2(Vec2, Vec2);
Vec3 vadd3(Vec3, Vec3);

typedef struct { Vec3 acc; Vec3 vel; Vec3 pos; } Projectile;
Projectile projectile(Vec3, Vec3, Vec3);

typedef struct { Vec3 pos; Vec2 a; Vec2 b; Vec2 c; Vec3 norm; /* Function reaction; */ } Triangle;
Triangle triangle(Vec3, Vec2, Vec2, Vec2, Vec3);
// pos is a point on the plane parallel to the triangle.  it will be in the same spot as the
//   centroid when drawn.
/*
a-b
|/
c
drawn on XZ-plane.
the three points must be posisitioned such that the centroid of the triangle is the origin.
*/
Vec2 centroid(Triangle);
Triangle t_centroid(Triangle);

typedef struct { Projectile p; // main 'ball' that is controlled.
                 std::vector<Triangle> t; /* tile for processing. */ } World;

#endif
