#include <SFML/Graphics.hpp>

#include <cmath>
#include <cstdio>
#include <cstring>

#include "world.hpp"
#include "bounds.hpp"
#include "physics.hpp"
#include "matrix.hpp"
#include "loader.hpp"

#define PI 3.14159265358979

#define DEBUG_MODE 1

// TODO: implement draw function for instanced drawing.
// NOTE: current number of rows and columns of grass blades is n = 100.
//     : the interval separating them is interval = 0.005 .

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
void d_triangulation(GLuint vao, int nsteps) {
  // assume total steps to be the square of nsteps.
  glBindVertexArray(vao);
  for(int i=0;i<nsteps;i++) { glDrawArrays(GL_TRIANGLE_STRIP,i*nsteps*2,nsteps*2); } }
void d_tri_instanced(VAOdat vaod, GLenum mode, int n) {
  glBindVertexArray(vaod.vao);
  for(int i=0;i<vaod.sz;i++) {
    glDrawArraysInstanced(mode,vaod.disp+i*vaod.nsteps,vaod.nsteps,n); } }
void d_triangulation_2(VAOdat vaod, GLenum mode) {
  glBindVertexArray(vaod.vao); // vaod.disp should be 5000, not 15000.
  for(int i=0;i<vaod.sz;i++) { glDrawArrays(mode,vaod.disp+i*vaod.nsteps,vaod.nsteps); } }

/* ==== entity draw test ========================================= */
int curr_id = 0;

#define EX_STEP (0.02)
#define EX_NSTEPS (250)
float ex_fun(float x, float z) { return sin(5.0*x)/5.0+sin(5.0*z)/5.0; }
float ex_fun_1(float x, float z) { return cos(5.0*x)/5.0+cos(5.0*z)/5.0; }
float ex_fun_2(float x, float z) { return pow(x-0.5,2.)+pow(z-0.5,2.); }
int ex_bounds(Vec2 a) { dist(a,v2(0.5,0.5))<0.5; }

// DONE: add bounds function for each entity so not all entities' triangles are calculated.

/* 2 4 6...
   |\|\|...
   1 3 5...
*/
// TODO: dedicated x-step and y-step as well as x-nsteps and y-nsteps.
std::vector<float> triangulate_0(FuncXZ f, float step, int nsteps) { int tsz;
  // every step has two points for drawing, both on the same x-coordinate.
  // six total floats for each step.
  std::vector<float> ret((tsz = nsteps*nsteps)*6);
  for(int i=0;i<nsteps;i++) {
    for(int j=0;j<nsteps*6;j+=6) { /* ||||| */ int ind = j+i*nsteps*6;
      ret[ind] = j/6*step; ret[ind+1] = f(j/6*step,i*step); ret[ind+2] = i*step;
      ret[ind+3] = j/6*step; ret[ind+4] = f(j/6*step,i*step+step); ret[ind+5] = i*step+step;
      /*printf("<%g, %g, %g> <%g, %g, %g>\n",ret[ind],ret[ind+1],ret[ind+2],ret[ind+3],ret[ind+4]
                                          ,ret[ind+5]);*/ } }
  return ret; }
std::vector<float> triangulate_1(FuncXZ f, float step, int nsteps) { int tsz;
  // every step has two points for drawing, both on the same x-coordinate.
  // six total floats for each step.  this is doubled for all associated normals.
  int floats_per_vert = 3;
  // one group is, in this case, the two vertices whose line is parallel to z-axis.
  //   e.g. |||||| : two vertices per line.
  int verts_per_group = 2;
  int n_attribs       = 2;

  // floats_per_group
  int fpg = floats_per_vert*verts_per_group*n_attribs; // = 12.

  std::vector<float> ret((tsz = nsteps*nsteps)*fpg);
  for(int i=0;i<nsteps;i++) { // just skips normals for now.
    for(int j=0;j<nsteps*fpg;j+=fpg) { /* ||||| */ int ind = j+i*nsteps*fpg;
      ret[ind] = j/fpg*step; ret[ind+1] = f(j/fpg*step,i*step); ret[ind+2] = i*step;
      ret[ind+6] = j/fpg*step; ret[ind+7] = f(j/fpg*step,i*step+step); ret[ind+8] = i*step+step;
      /*printf("<%g, %g, %g> <%g, %g, %g>\n",ret[ind],ret[ind+1],ret[ind+2],ret[ind+3],ret[ind+4]
                                          ,ret[ind+5]);*/ } }
  for(int i=0;i<102;i++) { printf("%g, ",ret[i]); }
  return ret; }

// expects model, view, and projection matrices to be named 'model', 'view', and 'projection'
//   in the shader.
// WARNING: current shader is still bound to 'prog' after call.
void mvp_set(GLuint prog, Matrix model, Matrix view, Matrix projection) {
  glUseProgram(prog);

  GLint _model = glGetUniformLocation(prog,"model");
  glUniformMatrix4fv(_model,1,GL_TRUE,&model.dat[0]);
  GLint _view = glGetUniformLocation(prog,"view");
  glUniformMatrix4fv(_view,1,GL_TRUE,&view.dat[0]);
  GLint _projection = glGetUniformLocation(prog,"projection");
  glUniformMatrix4fv(_projection,1,GL_TRUE,&projection.dat[0]); }
  

/* =============================================================== */

void paint(World *w,GLuint default_program) {
  glLoadIdentity(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glRotatef(30,-1,0,0);
  glUseProgram(w->e[0].shader_id);
  d_triangulation_2(w->e[0].vd,GL_TRIANGLE_STRIP);
  glUseProgram(w->e[1].shader_id);
  for(int i=1;i<6;i++) {
    GLint u_grass_id = glGetUniformLocation(w->e[1].shader_id,"u_grass_id");
    glUniform1i(u_grass_id,i);
    d_tri_instanced(w->e[i].vd,GL_TRIANGLE_STRIP,62500/5); }
  glUseProgram(default_program);
  /*d_square(w->p.pos.x-0.05,w->p.pos.y-0.05,w->p.pos.z-0.05,0.1);*/
  d_triangulation_2(w->e[6].vd,GL_TRIANGLE_FAN); }

Matrix gl_init(sf::Window *window) { glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClearDepth(1.f);
  glDepthFunc(GL_LESS);
  glDisable(GL_LIGHTING); /* temporary */ glViewport(0,0,window->getSize().x,window->getSize().y);
  //glMatrixMode(GL_PROJECTION); glLoadIdentity();
  float ratio = window->getSize().x/window->getSize().y;
  //glFrustum(-ratio,ratio,-1.f,1.f,1.f,500.f);
  return frustum_pers(-ratio,ratio,-1.f,1.f,1.f,500.f); }

// DONE: map through all triangles instead of just first.
// TODO: change projectile graphic with circle.
// TODO: increase performace of logic by having a bounding rectangle that encases each triangle.
//     : this will be the first pass of collision detection.

// TODO: make possible modification of the degradation (friction) coefficient for each
//     : entity.
// TODO: make possible to test for which side the ball falls for a polyhedron.

// DONE: turn triangulation function into actual triangles for collision.
//     : \|\|\|\|\|...
//       \|\|\|\|\|...
/*       .
         .
         .
*/

void handle_input(World *w, Matrix *model, Matrix *view, Matrix *projection) {
  // TODO: make cleaner code.
  Vec3 axis = v3(0,0,0);
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    axis = axis+v3(1,0,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    axis = axis+v3(-1,0,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    axis = axis+v3(0,1,0); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    axis = axis+v3(0,-1,0); }
  if(!(axis==v3(0,0,0))) { *model = rotate(*model,0.01,axis); }

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
    *model = scale(*model,v3(1.01,1.01,1.01)); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
    *model = scale(*model,v3(0.990099,0.990099,0.990099)); }

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
    *view = translate(*view,v3(0,0,0.01)); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
    *view = translate(*view,v3(0,0,-0.01)); }

  // TODO: make nicer.
  // DONE: change so it is only possible to add velocity if total velocity is below threshold.
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { w->tht += M_PI/100.; }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { w->tht -= M_PI/100.; }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { w->phi += M_PI/100.; }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { w->phi -= M_PI/100.; }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)&&len(w->p.vel)<0.0005) {
    Vec3 v = unit(v3(cos(w->tht)*cos(w->phi),sin(w->phi),sin(w->tht)*cos(w->phi)));
    w->p.vel = w->p.vel+1./25.*v; w->stroke++; }
  // TODO: add buffer between inputs so hole is not incremented every frame. 
  if(DEBUG_MODE&&sf::Keyboard::isKeyPressed(sf::Keyboard::N)) { unload(w,w->hole); load(w,++w->hole); }
  else if(DEBUG_MODE&&sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
    unload(w,w->hole); load(w,--w->hole); } }

// TODO: allow loading and unloading levels and reorganize program to make that more easily done.
int main() { sf::ContextSettings settings;
  settings.depthBits = 24; settings.stencilBits = 8; settings.antialiasingLevel = 0;
  settings.majorVersion = 3; settings.minorVersion = 3;
  sf::Window window(sf::VideoMode(800, 800), "hang", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true); Matrix projection = gl_init(&window);
  glewExperimental = GL_TRUE;
  glewInit();

  Matrix model = translate(id_mat(4),v3(-0.5,0,-1.0));
  Matrix view = translate(id_mat(4),v3(0,0,-5.5));
  //Matrix view = look_at(v3(0,0,-1),v3(0,0,0),v3(0,1,0));

  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->hole = 0;
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0.65,1,4.),0.05);
  w->stroke = 1; w->tht = 0; w->phi = 0;

  const float t0 = M_PI/10.; const float sz = 0.03;
  /*w->e = create_entities({ einit(rigid_elastic,triangulate(hole_0,EX_STEP,EX_NSTEPS),v3(0,0,0)
                                ,EX_STEP,EX_NSTEPS,EX_NSTEPS*2,true),
                           einit(no_react,grass_blade(sz,0.1,2,0,M_PI/2),v3(0,0,0)
                                ,0,1,52*detail+1,false),
                           einit(no_react,grass_blade(sz,0.1,2,t0,M_PI/4.),v3(0,0,0)
                                ,0,1,5,false),
                           einit(no_react,grass_blade(sz,0.1,2,t0,3*M_PI/4.),v3(0,0,0)
                                ,0,1,5,false),
                           einit(no_react,grass_blade(sz,0.1,2,t0,5*M_PI/4.),v3(0,0,0)
                                ,0,1,5,false),
                           einit(no_react,grass_blade(sz,0.1,2,t0,7*M_PI/4.),v3(0,0,0)
                                ,0,1,5,false),
                           einit(no_react,ball(0.05,20),v3(0,0,0),0,1,21,false) });
  // DONE: put all of this in new construction function.
  w->e[0].shader_id = create_program(sample_vs,sample_fs);
  w->e[1].shader_id = create_program(grass_vs,grass_fs);
  for(int i=2;i<6;i++) { w->e[i].shader_id = w->e[1].shader_id; }*/
  load(w,0);

  mvp_set(w->e[0].shader_id,model,view,projection);
  mvp_set(w->e[1].shader_id,model,view,projection);
  GLint u_res = glGetUniformLocation(w->e[0].shader_id,"u_res");
  glUniform2i(u_res,window.getSize().x,window.getSize().y);

  GLuint default_program = create_program(default_vs,default_fs);
  mvp_set(default_program,model,view,projection);
  int t = 0;
  for(bool r = true;r;t++) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    handle_input(w,&model,&view,&projection);
    // TODO: optimize this so it does not reset every frame.
    mvp_set(default_program,model,view,projection);
    GLint _pos = glGetUniformLocation(default_program,"pos");
    glUniform3f(_pos,w->p.pos.x,w->p.pos.y,w->p.pos.z);
    mvp_set(w->e[0].shader_id,model,view,projection);
    mvp_set(w->e[1].shader_id,model,view,projection);
    GLint _time = glGetUniformLocation(w->e[1].shader_id,"utime");
    glUniform1i(_time,t);

    Matrix model_3 = mtrunc(model);
    glUseProgram(w->e[0].shader_id);
    GLint _imod = glGetUniformLocation(w->e[0].shader_id,"u_imod");
    Matrix imod = minvert(model_3);
    glUniformMatrix4fv(_imod,1,GL_TRUE,&imod.dat[0]);
    paint(w,default_program); Projectile pp = next_state(w->p);
    /*printf("<%g,%g,%g>\n",w->p.pos.x,w->p.pos.y,w->p.pos.z);*/ entity_collide(w,pp,pl_side_ball_low);
    window.display(); } glDeleteVertexArrays(1,&w->e[0].vd.vao); return 0; }
