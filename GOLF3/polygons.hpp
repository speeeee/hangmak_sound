#include <SFML/Graphics.hpp>

#include "world.hpp"

/* ==== hole_0 ==== */
float hole_0(float, float);

std::vector<float> triangulate(FuncXZ, float, int);
std::vector<Triangle> to_triangles(std::vector<float>, float);
std::vector<Entity> create_entities(std::vector<EntBase>);
