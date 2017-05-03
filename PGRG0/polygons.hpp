#include "world.hpp"

std::vector<Vec3> triangulate(FuncXZ f, float xstep, float zstep, int xnsteps, int znsteps); 

std::vector<float> triangle_strip_surface(FuncXZ f, float xstep, float zstep, float xnsteps
                                         ,float znsteps, std::vector<Vec3> cols);
