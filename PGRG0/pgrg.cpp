#include <SFML/Graphics.hpp>

#include "world.hpp"
#include "shaders.hpp"
#include "util.hpp"
#include "polygons.hpp"
#include "matrix.hpp"

#include <cstdio>

#define DEBUG_MODE 1
#define PI 3.14159265358979323846264338

float fun(float x, float z) { return sin(x*z); }

// d_verts: draws from VAO:
void d_verts(VAOdat vd, GLenum mode) {
  glBindVertexArray(vd.vao); glDrawArrays(mode,vd.disp,vd.sz); }

void paint(std::vector<Entity> e) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(e[0].shader_id);
  d_verts(e[0].vd,GL_TRIANGLE_STRIP); }

Matrix gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDepthFunc(GL_LESS); glEnable(GL_CULL_FACE);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  //glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  //glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
  return frustum_pers(-ratio,ratio,-1.f,1.f,1.f,500.f); }

int main() { sf::ContextSettings settings;
  settings.depthBits = 24; settings.stencilBits = 8; settings.antialiasingLevel = 0;
  settings.majorVersion = 3; settings.minorVersion = 3;
  sf::Window window(sf::VideoMode(800,800), "pgrg", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true); glewExperimental = GL_TRUE;
  glewInit();

  Matrix model = rotate(id_mat(4),PI/4.,Vec3(1,0,0)); Matrix view = translate(id_mat(4),Vec3(0,0,-5.5));
  Matrix projection = gl_init(&window);

  GLuint default_program = create_program(dvs,dfs);
  mvp_set(default_program,model,view,projection);

  std::vector<Entity> ei = 
    create_entities({ triangle_strip_surface(fun,0.1,0.1,10,10
                        ,std::vector<Vec3>(90/* size*2 - xnsteps */,Vec3(1,1,1))) });
  ei[0].shader_id = default_program;

  int t = 0;
  for(bool r = true;r;t++) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(ei); window.display(); } glDeleteVertexArrays(1,&ei[0].vd.vao); return 0; }
