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
