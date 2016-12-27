#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

#include "world.hpp"

#define PI 3.14159265358979

Vec3 v3(float x, float y, float z) { return (Vec3) { x, y, z }; }
Vec2 v2(float x, float z) { return (Vec2) { x, z }; }
Triangle triangle(Vec2 a, Vec2 b, Vec2 c, Vec3 norm) { return (Triangle) { a, b, c, norm }; }

Vec2 centroid(Triangle a) { return v2((a.a.x+a.b.x+a.c.x)/3.,(a.a.z+a.b.z+a.c.z)/3.); }

Vec2 vsub2(Vec2 a, Vec2 b) { return v2(a.x-b.x,a.z-b.z); }

Triangle t_centroid(Triangle a) { Vec2 c = centroid(a);
  return triangle(vsub2(a.a,c),vsub2(a.b,c),vsub2(a.c,c),a.norm); }

Vec3 unit(Vec3 a) { float u = pow(pow(a.x,2)+pow(a.y,2)+pow(a.z,2),0.5);
  return (Vec3) { a.x/u, a.y/u, a.z/u }; }
float dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
float len(Vec3 a) { return pow(pow(a.x,2.)+pow(a.y,2.)+pow(a.z,2.),0.5); }
float angle(Vec3 a, Vec3 b) { float la = len(a); float lb = len(b);
  return acos(dot(a,b)/(la*lb)); }

Vec3 cross(Vec3 a, Vec3 b) {
  return (Vec3) { a.y*b.z-a.z*b.y, -a.x*b.z+a.z*b.x, a.x*b.y-a.y*b.x }; }

void d_v3(Vec3 a) { glVertex3f(a.x,a.y,a.z); }
void d_v2(Vec2 a) { glVertex3f(a.x,0,a.z); }
// requires origin be on centroid.
void d_tri(Triangle a) { glPushMatrix(); Vec3 c = cross(a.norm,v3(0,1,0));
  glRotatef(180/M_PI*angle(a.norm,v3(0,1,0)),-c.x,-c.y,-c.z);
  glBegin(GL_TRIANGLES); d_v2(a.a); d_v2(a.b); d_v2(a.c); glEnd(); glPopMatrix(); }

void paint(World *w) { glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1,0,0); glRotatef(10,-1,0,0); d_tri(w->t[0]); }

int main() {
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true);
  //sf::CircleShape shape(100.f);
  //shape.setFillColor(sf::Color::Blue);
  // TODO: fix angle.
  Triangle a = triangle(v2(0,0),v2(0,1),v2(1,0),v3(0.5,0.5,0));
  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->t.push_back(t_centroid(a));
  //glRotatef(30,-1,0,0);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w); window.display(); } return 0; }
  //while(window.isOpen()) { sf::Event event;
  //  while(window.pollEvent(event)) { if(event.type == sf::Event::Closed) { window.close(); } }
  //  window.clear(); window.draw(shape); window.display(); } return 0; }
