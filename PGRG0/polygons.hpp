#ifndef POLYGONS_HPP
#define POLYGONS_HPP
#include "world.hpp"

std::vector<Vec3> triangulate(FuncXZ f, float xstep, float zstep, int xnsteps, int znsteps);

// DONE: make this a template.
template <typename T>
std::vector<T> interpolate(T *a, T *b, int n);

std::vector<float> flatten(std::vector<Vec3> a);

std::vector<float> triangle_strip_surface(std::vector<Vec3> f, int xnsteps
                                         ,int znsteps, std::vector<Vec3> cols
                                         ,std::vector<Vec3> norms);
#define ERR (0.0001)
std::vector<float> triangle_strip_surface_function(FuncXZ f, float xstep, float zstep
                                                  ,int xnsteps, int znsteps, std::vector<Vec3> cols);

#endif
