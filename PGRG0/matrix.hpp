#ifndef MATRIX_HPP
#define MATRIX_HPP
#include "world.hpp"

#include <vector>

// written in row-major order, so transposition is required.

typedef struct { std::vector<float> dat; int r; int c; } Matrix;

Matrix matrix(std::vector<float>, int, int);
// 0x0 identity matrix represents error.
Matrix id_mat(int);
Matrix vec3_to_mat(Vec3);
Vec3 mat_to_vec3(Matrix);

Matrix madd(Matrix, Matrix);
Matrix operator+(Matrix, Matrix);
Matrix msub(Matrix, Matrix);
Matrix operator-(Matrix, Matrix);
Matrix mscalar(float, Matrix);
Matrix operator+(float, Matrix);

float dot_in_mat(float *, float *, int, int);
// assumes AxB * BxC
Matrix mmul(Matrix, Matrix);
Matrix operator*(Matrix, Matrix);
Matrix operator*(Matrix, Vec3);

Matrix mtrunc(Matrix);

//glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
// TODO: fix problem with narrowing conversion.
Matrix frustum_pers(float, float, float, float, float, float);

Matrix translate(Matrix, Vec3);
Matrix rotate(Matrix, float, Vec3);
Matrix transpose(Matrix);
Matrix scale(Matrix,Vec3);

Matrix look_at(Vec3, Vec3, Vec3);

void print_matrix(Matrix);

void col_in_4x4(Matrix, Matrix, int);
Matrix solve_tri(Matrix, Matrix, int);
// inefficient but works.
#define BACK  0
#define FRONT 1
// LU-decomposition using Crout matrix decomposition.
// expects three square matrices.
void lu_decomp(Matrix, Matrix *, Matrix *);

// TODO: inverse 4x4 matrix.
Matrix minvert(Matrix);
#endif
