//#ifdef WORLD_HPP
//#define WORLD_HPP

#include <vector>

typedef struct { float x; float y; float z; } Vec3;
Vec3 v3(float,float,float);
typedef struct { float x; float z; } Vec2;
Vec2 v2(float,float);
typedef struct { Vec3 acc; Vec3 vel; Vec3 pos; } Projectile;

typedef struct { Vec2 a; Vec2 b; Vec2 c; Vec3 norm; /* Function reaction; */ } Triangle;
Triangle triangle(Vec2, Vec2, Vec2, Vec3);
/*
a-b
|/
c
drawn on XZ-plane.
the three points must be posisitioned such that the centroid of the triangle is the origin.
*/

typedef struct { Projectile p; // main 'ball' that is controlled.
                 std::vector<Triangle> t; /* tile for processing. */ } World;

//#endif
