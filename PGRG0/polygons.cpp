#include "polygons.hpp"

#define ATTRIBS (3)

std::vector<Vec3> triangulate(FuncXZ f, float xstep, float zstep, int xnsteps, int znsteps) {
  std::vector<Vec3> ret(xnsteps*znsteps);
  for(int j=0;j<znsteps;j++) {
    for(int i=0;i<xnsteps;i++) { int ind = j*xnsteps+i; float px = xstep*i; float pz = zstep*j;
      ret[ind] = Vec3(px,f(px,pz),pz); } } return ret; }

// DONE: make this a template.
template <typename T>
std::vector<T> interpolate(T *a, T *b, int n) {
  std::vector<T> ret; // n+n
  for(int i=0;i<n;i++) { ret.push_back(a[i]); ret.push_back(b[i]); } return ret; }
// interp_stride: 'n' is meant to be the size of the smaller array.
template <typename T>
std::vector<T> interp_stride(std::vector<T> a, std::vector<T> b, int n, int stride) {
  std::vector<T> ret(a.size()+n); int j = 0; int k = 0; // not ideal code.
  for(int i=0;i<ret.size();i++) { if(!((i+1)%(stride+1))) { ret[i] = b[j++]; }
    else { ret[i] = a[k++]; } } return ret; }

// interp_tris: acts as a sliding window along z-axis.  all internal z-lines are duplicated.
// TODO: consider using VBOs instead.
std::vector<Vec3> interp_tris(std::vector<Vec3> a, int xnsteps, int znsteps) {
  std::vector<Vec3> ret;
  for(int i=0;i<znsteps-1;i++) {
    std::vector<Vec3> tx = interpolate(&a[xnsteps*i],&a[xnsteps*(i+1)],xnsteps);
    //printf("\n"); p_vec3_vec(tx);
    ret.insert(ret.end(),tx.begin(),tx.end()); } return ret; }

std::vector<float> flatten(std::vector<Vec3> a) { std::vector<float> ret;
  for(int i=0;i<a.size();i++) { ret.push_back(a[i].x); ret.push_back(a[i].y); ret.push_back(a[i].z); }
  return ret; }

std::vector<float> triangle_strip_surface(std::vector<Vec3> f, int xnsteps, int znsteps
                                         ,std::vector<Vec3> cols) {
  std::vector<Vec3> a = interp_tris(f,xnsteps,znsteps);
  std::vector<Vec3> cols_i = interp_tris(cols,xnsteps,znsteps);
  auto a_cols = interp_stride(a,cols_i,a.size(),1);
  // TODO: create normals from triangulation.
  auto a_cols_norms = interp_stride(a_cols,std::vector<Vec3>(a.size(),Vec3(0,0,0)),a.size(),2);
  // the following adds degenerate triangle to connect rows.
  // TODO: fix degenerate triangles
  /*for(int ind=2*xnsteps*ATTRIBS;ind<a_cols_norms.size();ind+=2*xnsteps*ATTRIBS) {
    p_vec3(a_cols_norms[ind]); p_vec3(a_cols_norms[ind+1]); p_vec3(a_cols_norms[ind+2]);
    p_vec3(a_cols_norms[ind-3]); p_vec3(a_cols_norms[ind-2]); p_vec3(a_cols_norms[ind-1]);
    Vec3 pos = Vec3(a_cols_norms[ind].x,a_cols_norms[ind-ATTRIBS].y,a_cols_norms[ind].z);
    std::vector<Vec3> e = { a_cols_norms[ind-3], a_cols_norms[ind-2], a_cols_norms[ind-1]
                          , pos, Vec3(0,0,0), Vec3(0,0,0), pos, Vec3(0,0,0), Vec3(0,0,0) };
    a_cols_norms.insert(a_cols_norms.begin()+ind,e.begin(),e.end()); ind+=ATTRIBS*3; }*/
  return flatten(a_cols_norms); }

std::vector<float> triangle_strip_surface_function(FuncXZ f, float xstep, float zstep
                                                  ,int xnsteps, int znsteps, std::vector<Vec3> cols) {
  return triangle_strip_surface(triangulate(f,xstep,zstep,xnsteps,znsteps),xnsteps,znsteps,cols); }
