#include <stdlib.h>
#include <glad/glad.h>

#define PLANE(NAME,NORMAL) GLfloat plane_ ## NAME(GLfloat x, GLfloat z) { \
  Vec3 normal = norm(NORMAL); return (-normal.x*x-normal.z*z)/normal.y; }
// FUN expected to be of type (GLfloat) ->  GLfloat
#define FUNCTION_JUST_X(NAME,FUN) GLfloat just_x_ ## NAME(GLfloat x, GLfloat z) { \
  return (FUN)(x); }
#define FUNCTION_JUST_Z(NAME,FUN) GLfloat just_z_ ## NAME(GLfloat x, GLfloat z) { \
  return (FUN)(z); }
#define MUL_CONSTANT(NAME,CONS,XZ) GLfloat mul_constant_ ## NAME(GLfloat x, GLfloat z) { \
  return (CONS)*(XZ); }

// both functions are (GLfloat) -> GLfloat.  done specifically in one of the above macros.
#define COMPOSE_X(FUN) (FUN)(x)
#define COMPOSE_Z(FUN) (FUN)(z)
#define COMPOSE(FUNA,FUNB) (FUNA)(FUNB)
// FUNC represents a function from (GLfloat) -> GLfloat, but is NOT actually a function pointer.
//   examples for a valid FUNC are 'x', 'COMPOSE_X(sin,x)'.
#define GEN_FUNCTION(NAME,FUNC) GLfloat func_ ## NAME(GLfloat x, GLfloat z) { \
  return FUNC; }

// TODO: make plane macro.
GLfloat plane_xz(GLfloat, GLfloat);
