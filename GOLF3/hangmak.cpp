#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <cmath>
#include <cstdio>
#include <cstring>

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

#define EX_STEP (0.1)
#define EX_NSTEPS (10)
typedef std::function<float(float, float)> FuncXZ;
float ex_fun(float x, float z) { return sin(5.0*x)/5.0+sin(5.0*z)/5.0; }
int ex_bounds(Vec2 a) { dist(a,v2(0.5,0.5))<0.5; }
int ex_bounds_2(Vec2 a) { return a.x>0&&a.x<0.5&&a.z>0&&a.z<0.5; }

std::vector<float> triangulate(FuncXZ f, float step, int nsteps) { int tsz;
  // every step has two points for drawing, both on the same z-coordinate.
  // six total floats for each step.
  std::vector<float> ret((tsz = nsteps*nsteps)*6);
  for(int i=0;i<nsteps;i++) {
    for(int j=0;j<nsteps*6;j+=6) { /* ||||| */ int ind = j+i*nsteps*6;
      ret[ind] = j/6*step; ret[ind+1] = f(j/6*step,i*step); ret[ind+2] = i*step;
      ret[ind+3] = j/6*step; ret[ind+4] = f(j/6*step,i*step+step); ret[ind+5] = i*step+step;
      /*printf("<%g, %g, %g> <%g, %g, %g>\n",ret[ind],ret[ind+1],ret[ind+2],ret[ind+3],ret[ind+4]
                                          ,ret[ind+5]);*/ } }
  return ret; }

// vector of buffers for bounds of entities.  deleted at end of program run.
std::vector<GLuint> bufs;

Entity sample_entity(CollisionF cf) {
  float ie[9] = { 0., 0., 0.,  0.5, 0., 0.,  0.5, 0., 0.5 };
  float *pts = new float[9]; memcpy(pts,ie,9*sizeof(float)); // not ideal solution, rushed.
  /*GLuint vao; GLuint vpts;
  glGenVertexArrays(1,&vao); glGenBuffers(1,&vpts);
  glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vpts);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid *)0);
    //glEnableVertexAttribArray(0);
  glBindVertexArray(0); bufs.push_back(vpts);*/
  return entity(v3(0,0,0),std::vector<Triangle>(),triangulate(ex_fun,EX_STEP,EX_NSTEPS),ex_bounds_2,cf,0); }

const GLchar *default_vs = //"#version 130 core\n"
  "void main() { gl_FrontColor = gl_Color;\n"
  "gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex; }\0";
const GLchar *default_fs = //"#version 130 core\n"
  "void main() { gl_FragColor = vec4(0.,1.,0.,1.); }\0";

const GLchar *sample_vs =
  "float samp_func(vec2 v) { return pow(v.x,2.); }\n"
  "void main() { vec4 v = vec4(gl_Vertex);\n"
  "  vec4 nv = vec4(v.x,samp_func(v.xz),v.z,v.w);\n"
  "  gl_Position = gl_ModelViewProjectionMatrix*v; }\0";
const GLchar *sample_fs =
  "void main() { gl_FragColor = vec4(1.,0.,0.,1.); }\0";

GLuint create_program(const GLchar *vsh, const GLchar *fsh) { GLuint vs;
  vs = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vs,1,&vsh,NULL); glCompileShader(vs);
  
  GLint compiled = 0;
  glGetShaderiv(vs,GL_COMPILE_STATUS,&compiled);
  if(!compiled) { GLint mlen = 0;
    glGetShaderiv(vs,GL_INFO_LOG_LENGTH, &mlen);
    std::vector<GLchar> error_log(mlen);
    glGetShaderInfoLog(vs,mlen,&mlen,&error_log[0]);
    glDeleteShader(vs); printf("%s\n",&error_log[0]); }
  GLuint fs; fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs,1,&fsh,NULL); glCompileShader(fs);
  compiled = 0;
  glGetShaderiv(fs,GL_COMPILE_STATUS,&compiled);
  if(!compiled) { GLint mlen = 0;
    glGetShaderiv(fs,GL_INFO_LOG_LENGTH, &mlen);
    std::vector<GLchar> error_log(mlen);
    glGetShaderInfoLog(fs,mlen,&mlen,&error_log[0]);
    glDeleteShader(fs); printf("%s\n",&error_log[0]); }

  GLuint prog; prog = glCreateProgram();
  glAttachShader(prog,vs); glAttachShader(prog,fs);
  glBindAttribLocation(prog,0,"position"); glLinkProgram(prog);
  glDeleteShader(vs); glDeleteShader(fs); return prog; }

GLfloat ie[9] = { 0., 0., 0.,  0.5, 0., 0.,  0.5, 0., 0.5 };
/* =============================================================== */

void paint(World *w,GLuint default_program) {
  glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glRotatef(30,-1,0,0);
  //glColor3f(1,0,0); //d_tris(w);
  //glBindVertexArray(w->e[0].vpts); glDrawArrays(GL_TRIANGLES,0,3);
  //glBindVertexArray(0);
  glUseProgram(w->e[0].shader_id);
  glTranslatef(-0.5,0.,-0.5);
  for(int i=0;i<EX_NSTEPS;i++) { glBegin(GL_TRIANGLE_STRIP); for(int j=0;j<EX_NSTEPS*2;j++) {
    glVertex3fv(&w->e[0].vpts[j*3+i*EX_NSTEPS*6]); } glEnd(); }
  glUseProgram(default_program);
  d_square(w->p.pos.x-0.05,w->p.pos.y-0.05,w->p.pos.z-0.05,0.1); }

void gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f); }

// DONE: map through all triangles instead of just first.
// TODO: change projectile graphic with circle.
// TODO: increase performace of logic by having a bounding rectangle that encases each triangle.
//     : this will be the first pass of collision detection.

// TODO: make possible modification of the degradation (friction) coefficient for each
//     : entity.
// TODO: make possible to test for which side the ball falls for a polyhedron.

// TODO: turn triangulation function into actual triangles for collision.
//     : \|\|\|\|\|...
//       \|\|\|\|\|...
/*       .
         .
         .
*/

int main() { sf::ContextSettings settings;
  settings.depthBits = 24; settings.stencilBits = 8; settings.antialiasingLevel = 0;
  settings.majorVersion = 3; settings.minorVersion = 0;
  sf::Window window(sf::VideoMode(200, 200), "hang", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true); gl_init(&window);
  glewInit();

  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0,1,0),0.05);
  /*w->e.push_back(entity(v3(0,0,0),std::vector<Triangle>(),0,ex_bounds,rigid_elastic,0));
  w->e[0].t.push_back(t_centroid(triangle(v3(0,0,0),v2(0,0),v2(0,1),v2(1,0),unit(v3(0.5,0.5,0)))));
  w->e[0].t.push_back(t_centroid(triangle(v3(sqrt(2)/2,0,0),v2(0,0),v2(0,0.5),v2(1,0),unit(v3(-0.5,0.5,0)))));*/
  w->e.push_back(sample_entity(rigid_elastic));
  w->e[0].shader_id = create_program(sample_vs,sample_fs);
  GLuint default_program = create_program(default_vs,default_fs);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    paint(w,default_program); Projectile pp = next_state(w->p); entity_collide(w,pp);
    window.display(); } return 0; }
