#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <memory>

#include <SFML/OpenGL.hpp>

#define GRAVITY (-0.0005)
#define DEGRADE (0.6)

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

Vec2 proj_xz(Vec3);

// multiplies each component of the two vectors together.
Vec3 vcmul(Vec3, Vec3);
// multiplies vector by scalar.
Vec3 vsmul(float, Vec3);

// negate components of vector
Vec3 vneg(Vec3);

float dist(Vec2, Vec2);

typedef struct { Vec3 acc; Vec3 vel; Vec3 pos; float rad; } Projectile;
Projectile projectile(Vec3, Vec3, Vec3, float);

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

typedef struct Entity Entity;
typedef struct { Projectile p; // main 'ball' that is controlled.
                 std::vector<Entity> e; /* tile for processing. */ } World;

typedef std::function<Projectile(World *, Triangle, Projectile)> CollisionF;
typedef std::function<int(Vec2)> BoundsF;

struct Entity { Vec3 pos; std::vector<Triangle> t; GLuint vpts;
                BoundsF bf; CollisionF cf; GLuint shader_id; };
Entity entity(Vec3, std::vector<Triangle>, GLuint, BoundsF, CollisionF, GLuint);
#endif
