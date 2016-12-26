//#ifdef WORLD_HPP
//#define WORLD_HPP

#include <vector>

typedef struct { float x; float y; float z; } Vec3;
Vec3 v3(float,float,float);
typedef struct { Vec3 acc; Vec3 vel; Vec3 pos; } Projectile;

typedef struct { float w; float h; Vec3 norm; /* Function reaction; */ } Triangle;
Triangle triangle(float, float, Vec3);
/*
x-x
|/
#
drawn on XZ-plane.
*/

typedef struct { Projectile p; // main 'ball' that is controlled.
                 std::vector<Triangle> t; /* tile for processing. */ } World;

//#endif
