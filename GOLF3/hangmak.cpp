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
  glTranslatef(a.pos.x,a.pos.y,a.pos.z);
  glRotatef(180/M_PI*angle(a.norm,v3(0,1,0)),-c.x,-c.y,-c.z);
  glBegin(GL_TRIANGLES); d_v2(a.a); d_v2(a.b); d_v2(a.c); glEnd(); glPopMatrix(); }
void d_tris(World *w) { for(int i=0;i<w->t.size();i++) { d_tri(w->t[i]); } }
void d_square(float x, float y, float z, float w) { glBegin(GL_QUADS);
  glVertex3f(x,y,z); glVertex3f(x+w,y,z); glVertex3f(x+w,y+w,z); glVertex3f(x,y+w,z);
  glEnd(); } // temporary function

void paint(World *w) { glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glRotatef(30,-1,0,0);
  glColor3f(1,0,0); d_tris(w);
  glColor3f(0,1,0); d_square(w->p.pos.x-0.05,w->p.pos.y-0.05,w->p.pos.z-0.05,0.1); }

void gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f); }

// DONE: map through all triangles instead of just first.
// TODO: change projectile graphic with circle.
// TODO: increase render performance keeping track of cross vector rather than calculating it
//     : each time.
// TODO: increase performace of logic by having a bounding rectangle that encases each triangle.
//     : this will be the first pass of collision detection.
// TODO: fix apparent error with collision with second triangle in test.

int main() {
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true); gl_init(&window);
  // DONE: fix angle.
  //Triangle a = triangle(v2(0,0),v2(0,1),v2(1,0),v3(0.5,0.5,0));
  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0,1,0),0.05);
  w->t.push_back(t_centroid(triangle(v3(0,0,0),v2(0,0),v2(0,1),v2(1,0),unit(v3(0.5,0.5,0)))));
  w->t.push_back(t_centroid(triangle(v3(sqrt(2)/2,0,0),v2(0,0),v2(0,0.5),v2(1,0),unit(v3(-0.5,0.5,0)))));
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w); Projectile pp = next_state(w->p); test_collide(w,pp,rigid_elastic);
    window.display(); } return 0; }
