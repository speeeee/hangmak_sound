#include "polygons.hpp"

// creates vertices for each point in the range specified.  They are sorted in ascending order:
//   (x0,z0), (x1,z0), (x2,z0), ... ,(xN,z0), (x0,z1), (x1,z1), ... ,(xN, zM)
// DONE_partial: add accompanying interpolate_vertices to create triangle strips.
std::vector<Vec3> triangulate(FuncXZ f, float xstep, float zstep, int xnsteps, int znsteps) {
  std::vector<Vec3> ret(xnsteps*znsteps);
  for(int j=0;j<znsteps;j++) {
    for(int i=0;i<xnsteps;i++) { int ind = j*xnsteps+i; float px = xstep*i; float pz = zstep*j;
      ret[ind] = Vec3(px,f(px,pz),pz); } } return ret; }

// DONE: make this a template.
template <typename T>
std::vector<T> interpolate(T *a, T *b, int n) {
  std::vector<T> ret(n*n);
  for(int i=0;i<n;i++) { ret.push_back(a[i]); ret.push_back(b[i]); } return ret; }
// interp_stride: 'n' is meant to be the size of the smaller array.
template <typename T>
std::vector<T> interp_stride(std::vector<T> a, std::vector<T> b, int n, int stride) {
  std::vector<T> ret = a;
  for(int i=stride;i<n*stride;i+=stride+1) { ret.insert(ret.begin()+i,b[i]); } return ret; }

std::vector<Vec3> interp_tris(std::vector<Vec3> a, int xnsteps, int znsteps) {
  std::vector<Vec3> ret;
  for(int i=0;i<znsteps-1;i++) {
    std::vector<Vec3> tx = interpolate(&a[xnsteps*i],&a[xnsteps*(i+1)],xnsteps);
    ret.insert(ret.end(),tx.begin(),tx.end()); } return ret; }

std::vector<float> flatten(std::vector<Vec3> a) { std::vector<float> ret;
  for(int i=0;i<a.size();i++) { ret.push_back(a[i].x); ret.push_back(a[i].y); ret.push_back(a[i].z); }
  return ret; }

std::vector<float> triangle_strip_surface(FuncXZ f, float xstep, float zstep, int xnsteps
                                         ,int znsteps, std::vector<Vec3> cols) {
  std::vector<Vec3> a = interp_tris(triangulate(f,xstep,zstep,xnsteps,znsteps),xnsteps,znsteps);
  auto a_cols = interp_stride(a,cols,a.size(),1);
  // TODO: create normals from triangulation.
  auto a_cols_norms = interp_stride(a_cols,std::vector<Vec3>(a.size(),Vec3(0,0,0)),a.size(),2);
  // adds degenerate triangle to connect rows.
  for(int i=2;i<znsteps-1;i++) { int ind = i*xnsteps*3;
    Vec3 pos = a_cols_norms[ind]; a_cols_norms.insert(a_cols_norms.begin()+ind,pos); }
  return flatten(a_cols_norms); }
