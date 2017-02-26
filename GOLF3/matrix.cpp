#include "matrix.hpp"
#include <cmath>

Matrix matrix(std::vector<float> dat, int r, int c) { return (Matrix) { dat, r, c }; }
// 0x0 identity matrix represents error.
Matrix id_mat(int sz) { Matrix a; a.r = a.c = sz;
  std::vector<float> dat(sz*sz,0.); for(int i=0;i<sz*sz;i+=sz+1) { dat[i] = 1; }
  a.dat = dat; return a; }
Matrix vec3_to_mat(Vec3 v, float w) { Matrix a; a.r = 4; a.c = 1; std::vector<float> dat(a.r*a.c,0.);
  dat[0] = v.x; dat[1] = v.y; dat[2] = v.z; dat[3] = w; a.dat = dat; return a; }
Vec3 mat_to_vec3(Matrix a) { return v3(a.dat[0],a.dat[1],a.dat[2]); }

Matrix madd(Matrix a, Matrix b) { Matrix ret = matrix(std::vector<float>(a.r*a.c),a.r,a.c);
  for(int i=0;i<b.r*b.c;i++) { ret.dat[i] = a.dat[i]+b.dat[i]; } return ret; }
Matrix operator+(Matrix a, Matrix b) { return madd(a,b); }
Matrix msub(Matrix a, Matrix b) { Matrix ret = matrix(std::vector<float>(a.r*a.c),a.r,a.c);
  for(int i=0;i<b.r*b.c;i++) { ret.dat[i] = a.dat[i]-b.dat[i]; } return ret; }
Matrix operator-(Matrix a, Matrix b) { return msub(a,b); }
Matrix mscalar(float a, Matrix b) { Matrix ret = matrix(std::vector<float>(b.r*b.c),b.r,b.c);
  for(int i=0;i<b.r*b.c;i++) { ret.dat[i] = a*b.dat[i]; } return ret; }
Matrix operator+(float a, Matrix b) { return mscalar(a,b); }

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
Matrix operator*(Matrix a, Matrix b) { return mmul(a,b); }
Matrix operator*(Matrix a, Vec3 b) { return mmul(a,vec3_to_mat(b,1)); }

Matrix mtrunc(Matrix a) { Matrix b = matrix(std::vector<float>((a.r-1)*(a.c-1)),a.r-1,a.c-1);
  for(int i=0;i<b.r*b.c;i++) { b.dat[i] = a.dat[i+i/a.c]; } return b; }

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

Matrix scale(Matrix a, Vec3 t) { std::vector<float> dat(a.r*a.c,0.);
  dat[0] = t.x; dat[a.c+1] = t.y; dat[a.c*2+2] = t.z; dat[a.c*3+3] = 1;
  return mmul(matrix(dat,4,4),a); }

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

// modifies first argument.
// expects matrix a and matrix col to have same column size.
void col_in_4x4(Matrix *a, Matrix col, int cn) {
  for(int i=0;i<col.r;i++) { a->dat[i*a->c+cn] = col.dat[i]; } }

// WARNING: not very optimized.
// TODO: make inversion given LUx = I where L and U are lower and upper triangle matrices
//     : respectively.
Matrix minvert(Matrix a) { Matrix l = matrix(std::vector<float>(a.r*a.c),a.r,a.c);
  Matrix u = matrix(std::vector<float>(a.r*a.c),a.r,a.c);
  lu_decomp(a,&l,&u); //Matrix col = matrix(std::vector<float>(a.r,1),a.r,1);
  Matrix id4 = id_mat(4); Matrix ret = id_mat(4);
  for(int i=0;i<a.r;i++) { Matrix q = matrix(std::vector<float>(a.r,0),a.r,1); q.dat[i] = 1;
    Matrix nl = solve_tri(l,q,FRONT);
    Matrix nu = solve_tri(u,nl,BACK);
    col_in_4x4(&ret,nu,i); } return ret; }

// solves a triangular matrix back/front substitution.
// expects square matrix (k x k) a, column vector (k x 1) col, and back/front.
// WARNING: returns newly allocated pointer.
Matrix solve_tri(Matrix a, Matrix col, int bf) {
  Matrix ret = matrix(std::vector<float>(a.r),a.r,1);
  // solves only front right now.
  // TODO: consolidate into non-repeated code.
  if(bf==FRONT) {
    for(int i=0;i<a.r;i++) { ret.dat[i] = col.dat[i]; int j;
      for(j=0;j<i;j++) { ret.dat[i] -= a.dat[i*a.r+j]*ret.dat[j]; }
      ret.dat[i] = ret.dat[i]/a.dat[i*a.r+j]; } return ret; }
  if(bf==BACK) {
    for(int i=a.r-1;i>=0;i--) { ret.dat[i] = col.dat[i]; int j;
      for(j=a.r-1;j>i;j--) { ret.dat[i] -= a.dat[i*a.r+j]*ret.dat[j]; }
      ret.dat[i] = ret.dat[i]/a.dat[i*a.r+j]; } return ret; }
  return id_mat(0); }

// NOTE: likely incorrect.
// adapted from Wikipedia example for Crout matrix decomposition.
void lu_decomp(Matrix ma, Matrix *ml, Matrix *mu) {
  float *a = &ma.dat[0]; float *l = &(ml->dat[0]); float *u = &(mu->dat[0]); int n = ma.r;
  for(int i=0;i<n;i++) { u[i*n+i] = 1; }
  for(int j=0;j<n;j++) { for(int i=j;i<n;i++) {
      float sum = 0; for(int k=0;k<j;k++) { sum += l[i*n+k] * u[k*n+j]; }
      l[i*n+j] = a[i*n+j] - sum; }
    for(int i=j;i<n;i++) { float sum = 0;
      for(int k=0;k<j;k++) { sum += l[j*n+k] * u[k*n+i]; }
      if(l[j*n+j]==0) { *ml = id_mat(0); *mu = id_mat(0); printf("failure\n"); return; /* dangerous for now */ }
      u[j*n+i] = (a[j*n+i]-sum)/l[j*n+j]; } } }
