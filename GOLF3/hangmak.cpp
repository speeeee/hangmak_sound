#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

#include "world.hpp"

#define PI 3.14159265358979

Vec3 v3(float x, float y, float z) { return (Vec3) { x, y, z }; }
Triangle triangle(float w, float h, Vec3 norm) { return (Triangle) { w, h, norm }; }

Vec3 unit(Vec3 a) { float u = pow(pow(a.x,2)+pow(a.y,2)+pow(a.z,2),0.5);
  return (Vec3) { a.x/u, a.y/u, a.z/u }; }
float dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
float len(Vec3 a) { return pow(pow(a.x,2.)+pow(a.y,2.)+pow(a.z,2.),0.5); }
float angle(Vec3 a, Vec3 b) { float la = len(a); float lb = len(b);
  return acos(dot(a,b)/(la*lb)); }

Vec3 cross(Vec3 a, Vec3 b) {
  return (Vec3) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }

void d_tri(Triangle a) { glPushMatrix(); Vec3 c = cross(a.norm,v3(0,1,0));
  glRotatef(180/M_PI*angle(a.norm,v3(0,1,0)),-c.x,-c.y,-c.z);
  glBegin(GL_TRIANGLES); glVertex3f(-0.5,0,-0.5); glVertex3f(-0.5,0,0.5);
    glVertex3f(0.5,0,-0.5); glEnd(); glPopMatrix(); }

void paint(World *w) { glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1,0,0); glRotatef(10,-1,0,0); d_tri(w->t[0]); }

int main() {
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true);
  //sf::CircleShape shape(100.f);
  //shape.setFillColor(sf::Color::Blue);
  // TODO: fix angle.
  World *w = new World(); w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  //glRotatef(30,-1,0,0);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w); window.display(); } return 0; }
  //while(window.isOpen()) { sf::Event event;
  //  while(window.pollEvent(event)) { if(event.type == sf::Event::Closed) { window.close(); } }
  //  window.clear(); window.draw(shape); window.display(); } return 0; }
