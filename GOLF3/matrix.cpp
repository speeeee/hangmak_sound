#include "matrix.hpp"
#include <cmath>

Matrix matrix(std::vector<float> dat, int r, int c) { return (Matrix) { dat, r, c }; }
// 0x0 identity matrix represents error.
Matrix id_mat(int sz) { Matrix a; a.r = a.c = sz;
  std::vector<float> dat(sz*sz,0.); for(int i=0;i<sz*sz;i+=sz+1) { dat[i] = 1; }
  a.dat = dat; return a; }

float dot_in_mat(float *a, float *b, int len, int col_sep) { float ret = 0;
  for(int i=0;i<len;i++) { ret += a[i]*b[i*col_sep]; } return ret; }
// assumes AxB * BxC
Matrix mmul(Matrix a, Matrix b) { if(a.c!=b.r) { return id_mat(0); }
  Matrix ret; ret.r = a.r; ret.c = b.c;
  std::vector<float> dat(a.r*b.c,0.);
  for(int i=0;i<a.r;i++) {
    for(int j=0;j<b.c;j++) {
      dat[j+i*b.c] = dot_in_mat(&a.dat[i*a.c],&b.dat[j],a.r,b.c); } }
  ret.dat = dat; return ret; }

//glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
Matrix frustum_pers(float left, float right, float bottom, float top, float near, float far) {
  Matrix ret; ret.r = ret.c = 4;
  std::vector<float> dat =
    { (float)2.*near/(right-left), 0, (right+left)/(right-left), 0
    , 0, (float)(2.*near/(top-bottom)), (top+bottom)/(top-bottom), 0
    , 0, 0, (float)(-(far+near)/(far-near))   , (float)-2.*far*near/(far-near)
    , 0, 0, -1, 0 };
  ret.dat = dat; return ret; }

Matrix translate(Matrix a, Vec3 t) {
  std::vector<float> dat =
    { 1, 0, 0, t.x
    , 0, 1, 0, t.y
    , 0, 0, 1, t.z
    , 0, 0, 0, 1 };
  return mmul(matrix(dat,4,4),a); }

Matrix rotate(Matrix a, float rad, Vec3 tt) {
  float c = cos(rad); float s = sin(rad); Vec3 t = unit(tt);
  std::vector<float> dat =
    { powf(t.x,2.)*(1-c)+c, t.x*t.y*(1-c)-t.z*s , t.x*t.z*(1-c)+t.y*s , 0
    , t.y*t.x*(1-c)+t.z*s , powf(t.y,2.)*(1-c)+c, t.y*t.z*(1-c)-t.x*s , 0
    , t.x*t.z*(1-c)-t.y*s , t.y*t.z*(1-c)+t.x*s , powf(t.z,2.)*(1-c)+c, 0
    , 0                   , 0                   , 0                   , 1 };
  return mmul(matrix(dat,4,4),a); }

Matrix transpose(Matrix a) { std::vector<float> dat(a.r*a.c,0.);
  for(int j=0;j<a.c;j++) { for(int i=0;i<a.r;i++) { dat[i+j*a.r] = a.dat[j+i*a.c]; } }
  return matrix(dat,a.c,a.r); }

Matrix look_at(Vec3 eye, Vec3 t, Vec3 up) {
  Vec3 z = unit(vsub3(eye,t));
  Vec3 x = unit(cross(up,z));
  Vec3 y = cross(z,x);

  std::vector<float> odat =
    { x.x, x.y, x.z, 0
    , y.x, y.y, y.z, 0
    , z.x, z.y, z.z, 0
    , 0  , 0  , 0  , 1 };
  /*std::vector<float> tdat =
    { 1, 0, 0, -eye.x
    , 0, 1, 0, -eye.y
    , 0, 0, 1, -eye.z
    , 0, 0, 0, 1 };*/
  return translate(matrix(odat,4,4),vneg(eye)); }

void print_matrix(Matrix a) {
  for(int j=0;j<a.r;j++) { printf("| ");
    for(int i=0;i<a.c;i++) { printf("%g ",a.dat[i+j*a.c]); } printf("|\n"); } }
