#ifndef WORLD_HPP
#define WORLD_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <memory>
#include <vector>

#define STRIDE (9) // (Position, Color, Normal)

typedef std::function<float(float, float)> FuncXZ;

struct Vec3 { float x; float y; float z;
              Vec3(float xe, float ye, float ze) { x = xe, y = ye; z = ze; }
              Vec3() {} };

Vec3 v3(float,float,float);
Vec3 cross(Vec3, Vec3);
Vec3 unit(Vec3);
float dot(Vec3, Vec3);
float len(Vec3);
float angle(Vec3 a, Vec3 b);
void p_vec3(Vec3);

Vec3 vsub3(Vec3, Vec3);

Vec3 vadd3(Vec3, Vec3);

Vec3 operator+(Vec3, Vec3);
Vec3 operator-(Vec3, Vec3);
bool operator==(Vec3, Vec3);

// multiplies each component of the two vectors together.
Vec3 vcmul(Vec3, Vec3);
// multiplies vector by scalar.
Vec3 vsmul(float, Vec3);
Vec3 operator*(float, Vec3);

// negate components of vector
Vec3 vneg(Vec3);

#endif
