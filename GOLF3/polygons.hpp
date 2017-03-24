#include <SFML/Graphics.hpp>

#include "world.hpp"

/* ==== hole_0 ==== */
float hole_0(float, float);
/* ==== hole_1 ==== */
float test_cyl_0(float, float);

std::vector<float> triangulate(FuncXZ, float, int);
std::vector<Triangle> to_triangles(std::vector<float>, float);
std::vector<Entity> create_entities(std::vector<EntBase>);
// ideally, tstep should be a factor of (ub-lb) (upper_bound-lower_bound).
std::vector<float> cyl_to_tris(FuncXZ, float, float, float, int, int);

std::vector<float> aappend(std::vector<float>, std::vector<float>);
std::vector<float> c_ring(std::vector<float>, float, float, int);
std::vector<float> c_clip(std::vector<float>, float);
