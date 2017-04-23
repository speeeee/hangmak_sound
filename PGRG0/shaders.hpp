#include "world.hpp"
#include "matrix.hpp"

GLuint create_program(const GLchar *vsh, const GLchar *fsh);
void mvp_set(GLuint prog, Matrix model, Matrix view, Matrix projection);
