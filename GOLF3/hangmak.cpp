#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <cmath>
#include <cstdio>

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
void d_tris(World *w) { for(int i=0;i<w->e.size();i++) {
  for(int j=0;j<w->e[i].t.size();j++) { d_tri(w->e[i].t[j]); } } }
void d_square(float x, float y, float z, float w) { glBegin(GL_QUADS);
  glVertex3f(x,y,z); glVertex3f(x+w,y,z); glVertex3f(x+w,y+w,z); glVertex3f(x,y+w,z);
  glEnd(); } // temporary function

/* ==== entity draw test ========================================= */
int curr_id = 0;

#define EX_STEP (0.01)
#define EX_NSTEPS (100)
typedef std::function<float(float, float)> FuncXZ;
float ex_fun(float x, float z) { sin(x)+sin(z); }
int ex_bounds(Vec2 a) { dist(a,v2(0.5,0.5))<0.5; }

// very rushed code, TODO format.
std::vector<Triangle> triangulate(FuncXZ f, float step, int nsteps) {
  std::vector<Triangle> ret; int tns = 2*nsteps;
  for(int i=0;i<tns;i++) { Triangle a; Triangle b;
    Vec3 va = v3(0,0,f((i%nsteps)*step,(i/nsteps)*step));
    Vec3 vb = v3(step,0,f((i%nsteps)*step+step,(i/nsteps)*step));
    Vec3 vc = v3(0,step,f((i%nsteps)*step,(i/nsteps)*step+step));
    Vec3 vd = v3(step,step,f((i%nsteps)*step+step,(i/nsteps)*step+step));

    a.norm = unit(cross(vb,vc)); b.norm = unit(cross(v3(0,vb.y,step),v3(step,vc.y,0)));
    // note: the position is not needed in the centroid calculation, so there is no dependency lock.
    Vec2 nposa = vsub2(v2((i%nsteps)*step,(i/nsteps)*step),centroid(a));
    Vec2 nposb = vsub2(v2((i%nsteps)*step+step,(i/nsteps)*step+step),centroid(a));
    a.pos = v3(nposa.x,0,nposa.z); b.pos = v3(nposb.x,0,nposb.z);

    ret.push_back(a); ret.push_back(b); } return ret; }

// vector of buffers for bounds of entities.  deleted at end of program run.
std::vector<float *> bufs;

Entity read_entity(const char *e, CollisionF cf) {
  FILE *f = fopen(e,"rb"); Vec3 pos;
  // a little rough here, but will clean up later.
  fread(&pos.x,sizeof(float),1,f);
  fread(&pos.y,sizeof(float),1,f);
  fread(&pos.z,sizeof(float),1,f);
  int amt; fread(&amt,sizeof(int),1,f);
  float *pts = new float[amt*3];
  for(int i=0;i<amt;i+=3) { float x; float z;
    fread(&x,sizeof(float),1,f);
    fread(&z,sizeof(float),1,f); pts[i] = x; pts[i+1] = 0; pts[i+2] = z; }
  // bind buffer data.
  bufs.push_back(&pts[0]);
  GLuint vpts; glGenBuffers(1,&vpts); glBindBuffer(GL_ARRAY_BUFFER, vpts);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
  return entity(pos,triangulate(ex_fun,EX_STEP,EX_NSTEPS),vpts,ex_bounds,cf,curr_id++);
  /* modifies current ID. */ }

/* =============================================================== */

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
// DONE: fix apparent error with collision with second triangle in test.

// TODO: make possible modification of the degradation (friction) coefficient for each
//     : entity.
// TODO: make possible to test for which side the ball falls for a polyhedron.
// TODO: make polygon composable of connected functions.  allow its triangularization to an
//     : arbitrary degree and use a 3D function within the polygon's bounds to displace each vertex
//     : corresponding to its xz position using the vertex shader.

int main() {
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true); gl_init(&window);
  glewInit();
  // DONE: fix angle.
  //Triangle a = triangle(v2(0,0),v2(0,1),v2(1,0),v3(0.5,0.5,0));
  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0,1,0),0.05);
  w->e.push_back(entity(v3(0,0,0),std::vector<Triangle>(),0,ex_bounds,rigid_elastic,0));
  w->e[0].t.push_back(t_centroid(triangle(v3(0,0,0),v2(0,0),v2(0,1),v2(1,0),unit(v3(0.5,0.5,0)))));
  w->e[0].t.push_back(t_centroid(triangle(v3(sqrt(2)/2,0,0),v2(0,0),v2(0,0.5),v2(1,0),unit(v3(-0.5,0.5,0)))));
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w); Projectile pp = next_state(w->p); entity_collide(w,pp); //test_collide(w,pp,rigid_elastic);
    window.display(); } return 0; }
