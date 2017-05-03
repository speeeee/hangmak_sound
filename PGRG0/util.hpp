#include "world.hpp"

struct VAOdat { int disp; int sz; GLuint vao;
                VAOdat(int _disp, int _sz, GLuint _vao) { disp = _disp; sz = _sz; vao = _vao; }
                VAOdat() {} };
struct Entity { VAOdat vd; Entity(VAOdat _vd) { vd = _vd; }
                           Entity() {} }; // to be expanded.

std::vector<Entity> create_entities(std::vector<std::vector<float>>);
