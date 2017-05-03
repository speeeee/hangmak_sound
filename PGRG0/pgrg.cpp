#include <SFML/Graphics.hpp>

#include "world.hpp"
#include "shaders.hpp"
#include "util.hpp"

#define DEBUG_MODE 1

/*static const GLchar *dvs = "#version 330\n"
  "layout (location = 0) in vec3 position;\n"
  "layout (location = 1) in vec3 color;\n"
  "layout (location = 2) in vec3 norm;\n"*/

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

  int t = 0;
  for(bool r = true;r;t++) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); window.display(); } return 0; }
