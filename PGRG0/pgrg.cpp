#include <SFML/Graphics.hpp>

#include "world.hpp"
#include "shaders.hpp"
#include "util.hpp"
#include "polygons.hpp"
#include "matrix.hpp"

#include <cstdio>
#include <cassert>

#define DEBUG_MODE 1
#define PI 3.14159265358979323846264338

float fun(float x, float z) { return sin(x*z); }

// d_verts: draws from VAO:
void d_verts(VAOdat vd, GLenum mode) {
  glBindVertexArray(vd.vao); glDrawArrays(mode,vd.disp,vd.sz); }
// TODO: possibly make better fix?
void d_rows(VAOdat vd, int xnsteps, int znsteps, GLenum mode) {
  glBindVertexArray(vd.vao);
  for(int i=0;i<znsteps;i++) { glDrawArrays(mode,vd.disp+i*xnsteps*2,xnsteps*2); } }

void paint(std::vector<Entity> e) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(e[0].shader_id);
  d_rows(e[0].vd,10,10,GL_TRIANGLE_STRIP); }

Matrix gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDepthFunc(GL_LESS); glEnable(GL_CULL_FACE);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  //glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  //glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
  return frustum_pers(-ratio,ratio,-1.f,1.f,1.f,500.f); }

void handle_input(Matrix *model, Matrix *view, Matrix *projection) {
  // TODO: make cleaner code.
  Vec3 axis = Vec3(0,0,0);
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    axis = axis+Vec3(1,0,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    axis = axis+Vec3(-1,0,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    axis = axis+Vec3(0,1,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    axis = axis+Vec3(0,-1,0); }
  if(!(axis==Vec3(0,0,0))) { *model = rotate(*model,0.01,axis); } }

int main() { sf::ContextSettings settings;
  settings.depthBits = 24; settings.stencilBits = 8; settings.antialiasingLevel = 0;
  settings.majorVersion = 3; settings.minorVersion = 3;
  sf::Window window(sf::VideoMode(800,800), "pgrg", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true); glewExperimental = GL_TRUE;
  Matrix projection = gl_init(&window);
  glewInit();

  Matrix model = rotate(id_mat(4),PI/4.,Vec3(1,0,0)); Matrix view = translate(id_mat(4),Vec3(0,0,-5.5));

  GLuint default_program = create_program(dvs,dfs);
  mvp_set(default_program,model,view,projection);

  std::vector<Entity> ei = 
    create_entities({ triangle_strip_surface_function(fun,0.1,0.1,10,10
                        ,std::vector<Vec3>(100/* 2*(xsz*ysz - xnsteps) */,Vec3(1,1,1))) });
  ei[0].shader_id = default_program;

  int t = 0;
  for(bool r = true;r;t++) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    handle_input(&model,&view,&projection);
    for(int i=0;i<ei.size();i++) { mvp_set(ei[i].shader_id,model,view,projection); }
    paint(ei); window.display(); } glDeleteVertexArrays(1,&ei[0].vd.vao); return 0; }
