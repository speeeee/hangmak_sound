#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

#include "world.hpp"
#include "bounds.hpp"
#include "physics.hpp"

#define PI 3.14159265358979

void d_v3(Vec3 a) { glVertex3f(a.x,a.y,a.z); }
void d_v2(Vec2 a) { glVertex3f(a.x,0,a.z); }
// requires origin be on centroid.
// finds cross product of normal and Y-axis and rotates along the resulting axis the angle between
//   the normal and the Y-axis.
void d_tri(Triangle a) { glPushMatrix(); Vec3 c = cross(a.norm,v3(0,1,0));
  glRotatef(180/M_PI*angle(a.norm,v3(0,1,0)),-c.x,-c.y,-c.z);
  glBegin(GL_TRIANGLES); d_v2(a.a); d_v2(a.b); d_v2(a.c); glEnd(); glPopMatrix(); }
void d_square(float x, float y, float z, float w) { glBegin(GL_QUADS);
  glVertex3f(x,y,z); glVertex3f(x+w,y,z); glVertex3f(x+w,y+w,z); glVertex3f(x,y+w,z);
  glEnd(); } // temporary function

void paint(World *w) { glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glRotatef(30,-1,0,0);
  glColor3f(1,0,0); d_tri(w->t[0]);
  glColor3f(0,1,0); d_square(w->p.pos.x-0.05,w->p.pos.y-0.05,w->p.pos.z-0.05,0.1); }

void gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f); }

int main() {
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true); gl_init(&window);
  //sf::CircleShape shape(100.f);
  //shape.setFillColor(sf::Color::Blue);
  // TODO: fix angle.
  //Triangle a = triangle(v2(0,0),v2(0,1),v2(1,0),v3(0.5,0.5,0));
  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0,1,0));
  w->t.push_back(t_centroid(triangle(v3(0,0,0),v2(0,0),v2(0,1),v2(1,0),unit(v3(0.5,0.5,0)))));
  //glRotatef(30,-1,0,0);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w); Projectile pp = next_state(w->p);
    if(in_triangle(proj_xz(pp.pos),w->t[0])&&pl_side(w->p.pos,pp.pos,w->t[0])) {
      // pp.vel = pp.vel - 2(pp.vel . normal)normal
      pp.vel = vsub3(pp.vel,vsmul(2*dot(pp.vel,w->t[0].norm),w->t[0].norm));
      w->p.acc = pp.acc; w->p.vel = pp.vel; }
    else { w->p = pp; } window.display(); } return 0; }
  //while(window.isOpen()) { sf::Event event;
  //  while(window.pollEvent(event)) { if(event.type == sf::Event::Closed) { window.close(); } }
  //  window.clear(); window.draw(shape); window.display(); } return 0; }
