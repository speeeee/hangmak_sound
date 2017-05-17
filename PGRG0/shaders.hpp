#include "world.hpp"
#include "matrix.hpp"

static const GLchar *dvs = "#version 330\n"
  "layout (location = 0) in vec3 position;\n"
  "layout (location = 1) in vec3 color;\n"
  "layout (location = 2) in vec3 norm;\n"
  //"uniform vec3 disp;\n"
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection;\n"
  "void main() { gl_Position = projection*view*model*vec4(position,1.); }\0";
static const GLchar *dfs = "#version 330\n"
  "void main() { gl_FragColor = vec4(0.1,0.5,0.7,1.); }\0";

GLuint create_program(const GLchar *vsh, const GLchar *fsh);
void mvp_set(GLuint prog, Matrix model, Matrix view, Matrix projection);
