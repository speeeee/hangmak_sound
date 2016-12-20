#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

void paint() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBegin(GL_TRIANGLES); glColor3f(1,0,0);
  glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(0,1,0); glEnd(); }

int main() {
  sf::Window window(sf::VideoMode(200, 200), "SFLMLE");
  window.setVerticalSyncEnabled(true);
  //sf::CircleShape shape(100.f);
  //shape.setFillColor(sf::Color::Blue);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(); window.display(); } return 0; }
  //while(window.isOpen()) { sf::Event event;
  //  while(window.pollEvent(event)) { if(event.type == sf::Event::Closed) { window.close(); } }
  //  window.clear(); window.draw(shape); window.display(); } return 0; }
