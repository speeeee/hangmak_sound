#include "world.hpp"

// written in row-major order, so transposition is required.

typedef struct { std::vector<float> dat; int r; int c; } Matrix;

Matrix matrix(std::vector<float>, int, int);
// 0x0 identity matrix represents error.
Matrix id_mat(int);

float dot_in_mat(float *, float *, int, int);
// assumes AxB * BxC
Matrix mmul(Matrix, Matrix);

//glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
// TODO: fix problem with narrowing conversion.
Matrix frustum_pers(float, float, float, float, float, float);

Matrix translate(Matrix, Vec3);

Matrix rotate(Matrix, float, Vec3);

Matrix transpose(Matrix);
