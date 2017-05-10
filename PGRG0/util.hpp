#include "world.hpp"

struct VAOdat { int disp; int sz; GLuint vao;
                VAOdat(int _disp, int _sz, GLuint _vao) { disp = _disp; sz = _sz; vao = _vao; }
                VAOdat() {} };
struct Entity { VAOdat vd; GLuint shader_id; Entity(VAOdat _vd, GLuint _si) { vd = _vd; shader_id = _si; }
                           Entity() {} }; // to be expanded.

std::vector<Entity> create_entities(std::vector<std::vector<float>>);
