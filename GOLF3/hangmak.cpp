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
#include "matrix.hpp"

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
// DONE: switch triangulation to retained mode with VAO.
/*void d_triangulation(std::vector<float> vpts, int nsteps) {
  // in immediate mode right now, but will switch to drawing from a VAO.
  // recall that for every step, there are 2 points drawn.
  for(int i=0;i<nsteps;i++) { glBegin(GL_TRIANGLE_STRIP); for(int j=0;j<nsteps*2;j++) {
    glVertex3fv(&vpts[j*3+i*nsteps*6]); } glEnd(); } }*/
void d_triangulation(GLuint vao, int nsteps) {
  // assume total steps to be the square of nsteps.
  glBindVertexArray(vao);
  for(int i=0;i<nsteps;i++) { glDrawArrays(GL_TRIANGLE_STRIP,i*nsteps*2,nsteps*2); } }
void d_triangulation_2(VAOdat vaod) {
  glBindVertexArray(vaod.vao); // vaod.disp should be 5000, not 15000.
  for(int i=0;i<vaod.sz;i++) { glDrawArrays(GL_TRIANGLE_STRIP,vaod.disp+i*vaod.nsteps,vaod.nsteps); } }

/* ==== entity draw test ========================================= */
int curr_id = 0;

#define EX_STEP (0.02)
#define EX_NSTEPS (250)
typedef std::function<float(float, float)> FuncXZ;
float ex_fun(float x, float z) { return sin(5.0*x)/5.0+sin(5.0*z)/5.0; }
float ex_fun_1(float x, float z) { return cos(5.0*x)/5.0+cos(5.0*z)/5.0; }
float ex_fun_2(float x, float z) { return pow(x-0.5,2.)+pow(z-0.5,2.); }
float hole_0(float x, float z) { return 1./(2*(1+exp(-5*(-(z-3+pow(x-3.8,2.)/2.)))))
                                        +sin(4*x)/8+cos(3*x)/12+sin(3*z)/8
                                        +cos(5*z)/12; }
int ex_bounds(Vec2 a) { dist(a,v2(0.5,0.5))<0.5; }
int ex_bounds_2(Vec2 a) { return a.x>0&&a.x<5.0&&a.z>0&&a.z<5.0; }

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
// TODO: needs significant refactoring.
std::vector<float> triangulate(FuncXZ f, float step, int nsteps) { int tsz;
  // every step has two points for drawing, both on the same x-coordinate.
  // six total floats for each step.  this is doubled for all associated normals.
  int floats_per_vert = 3;
  // one group is, in this case, the two vertices whose line is parallel to z-axis.
  //   e.g. |||||| : two vertices per line.
  int verts_per_group = 2;
  int n_attribs       = 2;

  // attributes per vertex * floats_per_vert
  int apv = floats_per_vert*n_attribs; // = 6

  // floats_per_group
  int fpg = floats_per_vert*verts_per_group*n_attribs; // = 12.

  std::vector<float> ret((tsz = nsteps*nsteps)*fpg);
  for(int i=0;i<nsteps;i++) { int d = i*nsteps*fpg;
    // first triangle case:
    ret[d] = 0; ret[d+1] = f(0,i*step); ret[d+2] = i*step;
    ret[d+6] = 0; ret[d+7] = f(0,i*step+step); ret[d+8] = i*step+step;
    ret[d+12] = step; ret[d+13] = f(step,i*step); ret[d+14] = i*step;
    Vec3 a = arr_to_vec(&ret[d]); Vec3 b = arr_to_vec(&ret[d+6]); Vec3 c = arr_to_vec(&ret[d+12]);
    Vec3 norm = unit(cross(b-a,c-a));
    ret[d+3] = norm.x; ret[d+4] = norm.y; ret[d+5] = norm.z;
    ret[d+9] = norm.x; ret[d+10] = norm.y; ret[d+11] = norm.z;
    ret[d+15] = norm.x; ret[d+16] = norm.y; ret[d+17] = norm.z;
    for(int j=apv*3;j<nsteps*fpg;j+=apv) { int ind = j+d;
      if((j/apv)%2) { ret[ind+1] = f(j/fpg*step,i*step+step); ret[ind+2] = i*step+step; }
      else { ret[ind+1] = f(j/fpg*step,i*step); ret[ind+2] = i*step; }
      ret[ind] = j/fpg*step;

      Vec3 a = arr_to_vec(&ret[ind]); Vec3 b = arr_to_vec(&ret[ind-apv]);
      Vec3 c = arr_to_vec(&ret[ind-apv*2]);
      Vec3 v0 = b-a; Vec3 v1 = c-a;
      if((j/apv)%2) { v0.x = -v0.x; v0.z = -v0.z; v0.y = -v0.y;
                      v1.x = -v1.x; v1.z = -v1.z; v1.y = -v1.y; }
      else { v0.x = -v0.x; v0.y = -v0.y;
             v1.x = -v1.x; v1.y = -v1.y; }
      norm = unit(cross(v0,v1)); 
      ret[ind+3] = norm.x; ret[ind+4] = norm.y; ret[ind+5] = norm.z; } }
  return ret; }
// TODO: dedicated x-step and y-step.
std::vector<Triangle> to_triangles(std::vector<float> arr, float step) {
  int floats_per_vert = 3;
  // one group is, in this case, the two vertices whose line is parallel to z-axis.
  //   e.g. |||||| : two vertices per line.
  int verts_per_group = 2;
  int n_attribs       = 2;

  // attributes per vertex * floats_per_vert
  int apv = floats_per_vert*n_attribs; // = 6

  // floats_per_group
  int fpg = floats_per_vert*verts_per_group*n_attribs; // = 12.

  std::vector<Triangle> ret;
  for(int i=0;i<arr.size();i+=fpg) { Triangle a, b;
    a.a = v2(0,0); a.b = v2(0,step); a.c = v2(step,0);
    b.a = v2(step,step); b.b = v2(step,0); b.c = v2(0,step);

    Vec3 bl = arr_to_vec(&arr[i]); Vec3 br = arr_to_vec(&arr[i+apv*2]);
    Vec3 tr = arr_to_vec(&arr[i+apv*3]); Vec3 tl = arr_to_vec(&arr[i+apv*1]);

    // TODO: chage the normal vector calculation since it is already stored now.
    a.norm = unit(cross(vsub3(tl,bl),vsub3(tr,bl)));
    b.norm = unit(vneg(cross(vsub3(tl,tr),vsub3(br,tr))));
    a.pos = bl; b.pos = tr;
    // expects leg on x-axis first, and then leg on z-axis.
    ret.push_back(a);
    ret.push_back(b); }
  /*for(int i=0;i<ret.size();i++) {
    printf("%i: <%g, %g, %g> <%g, %g, %g>\n",i,ret[i].pos.x,ret[i].pos.y,ret[i].pos.z
          ,ret[i].norm.x,ret[i].norm.y,ret[i].norm.z); }*/ return ret; }

// vector of buffers for bounds of entities.  deleted at end of program run.
std::vector<GLuint> bufs;

// DONE: create_entities: store entities created into single VAO at different displacements.
//     : struct VAOdat { int disp; int sz; GLuint vao; };
// TODO: change to not force function-based render.
// TODO: change EntInit to <CollisionF,std::vector<float>>.  direct to triangulate.
//typedef std::tuple<CollisionF,FuncXZ,Vec3,float,int> EntInit;
//EntInit einit(CollisionF cf, FuncXZ f, Vec3 pos, float step, int nsteps) {
//  return std::make_tuple(cf,f,pos,step,nsteps); }
// DONE: create VAOdat for given input.
std::vector<Entity> create_entities(std::vector<EntBase> ei) { std::vector<Entity> ret;
  // assumes stride of 6 currently.
  std::vector<float> dat;
  GLuint vao; glGenVertexArrays(1,&vao);
  for(int i=0;i<ei.size();i++) {
    CollisionF cf = std::get<0>(ei[i]); std::vector<float> tris = std::get<1>(ei[i]);
    Vec3 pos = std::get<2>(ei[i]); float step = std::get<3>(ei[i]); int nsteps = std::get<4>(ei[i]);

    std::vector<Triangle> btris = to_triangles(tris,step);
    asadd(pos,&tris[0],tris.size(),6); // for stride.

    if(std::get<5>(ei[i])) {
      // calculation for vaod.pos is dat.size()/stride
      //   (dat.size() is guaranteed to be a multiple of stride).
      ret.push_back(entity(pos,btris,tris,vao_dat(dat.size()/6,nsteps,nsteps*2,vao)
                          ,ex_bounds_2,cf,0)); }
    /* append to dat vector tris */
    dat.reserve(dat.size()+tris.size());
    dat.insert(dat.end(),tris.begin(),tris.end()); }
  glBindVertexArray(vao);
  GLuint buf; glGenBuffers(1,&buf);
  glBindBuffer(GL_ARRAY_BUFFER, buf);
  glBufferData(GL_ARRAY_BUFFER,dat.size()*sizeof(float),&dat[0],GL_STATIC_DRAW);
  // assume limit is 16.
  GLuint pos_attrib = 0; GLuint norm_attrib = 1;
  glEnableVertexAttribArray(pos_attrib); // attribute location, e.g. (location = 0) for position.
  glVertexAttribPointer(pos_attrib,3,GL_FLOAT,GL_FALSE,6*sizeof(GL_FLOAT),0); // reminder: first argument requires
                                                    //   glBindAttribLocation(..).
  glEnableVertexAttribArray(norm_attrib);
  glVertexAttribPointer(norm_attrib,3,GL_FLOAT,GL_FALSE,6*sizeof(GL_FLOAT)
                       ,(const GLvoid *)(3*sizeof(GL_FLOAT)));

  return ret; }

// TODO: rewrite for circle drawing.  Translate circle based off of given uniform position vector.
//     : rotate to always face camera.  Shade based off of global light.
const GLchar *default_vs = "#version 330\n"
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection;\n"
  "void main() { gl_FrontColor = gl_Color;\n"
  "gl_Position = projection*view*model*gl_Vertex; }\0";
const GLchar *default_fs = "#version 330\n"
  "void main() { gl_FragColor = vec4(0.,1.,0.9,1.); }\0";

// DONE: put MVP and inverse-P in fragment shader for computing eye-position.
// NOTE: in glVertexAttribPointer: first argument is the attribute (set position = 0).
const GLchar *sample_vs = "#version 330\n"
  "layout (location = 0) in vec3 position; out vec3 frag_pos;\n"
  "layout (location = 1) in vec3 norm; out vec3 frag_norm;\n"
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection;\n"
  "out mat4 frag_model;\n"
  "float samp_func(vec2 v) { return pow(v.x,2.); }\n"
  "void main() {\n"
  "  frag_pos = position; frag_norm = norm;\n"
  "  gl_Position = projection*view*model*vec4(position.xyz,1.0); }\0";
/* DONE: add normals for each vertex to triangulate, scaling stride to 3*sizeof(GLfloat),
   *       array is now made to fit the normal vectors, but the normal is not created yet.
       : pass normals to vertex shader as (location = 1) and forward to fragment shader,
       : perform lighting calculations. */
/* CHANGES SO FAR:
     add input vector 'norm' where (location = 1) (through glBindAttribLocation)
     'norm' must be enabled and have been assigned by glVertexAttribPointer.
   DONE* move location binding and attrib pointer assignment to isolated function. */

// TODO: add first course with shader for the bounds of each type of terrain.
// TODO: add grass effect (perlin noise?).
const GLchar *sample_fs = "#version 330\n"
  "uniform ivec2 u_res;\n"
  //"uniform mat4 imvp;\n"
  "uniform mat3 u_imod;\n"
  "in mat4 frag_model;\n"
  "in vec3 frag_pos; in vec3 frag_norm;\n"
  /*"void main() { vec4 ndc_pos; ndc_pos.xy = (2.0*gl_FragCoord.xy)/(u_res.xy)-1;\n"
  "  ndc_pos.z = (2.0*gl_FragCoord.z-gl_DepthRange.near-gl_DepthRange.far) /"
  "              (gl_DepthRange.far - gl_DepthRange.near);\n"
  "  ndc_pos.w = 1.0; vec4 clip_pos = ndc_pos / gl_FragCoord.w;\n"
  "  vec4 pos = imvp*clip_pos;\n"*/
  // as isz grows, the curve gets smaller.
  "float curve_0(float x, float isz) {\n"
  "  return (sqrt(1-pow(x,2.)/pow(2./isz,2.))+2/(1+exp(-6.*isz*x)))/(isz*2.); }\n"
  "void main() {\n"
  "  vec3 light = normalize(vec3(0.,-1.,0.));\n" // example light
  "  float brightness = dot(-light,frag_norm);\n"
  "  vec3 color = vec3(0.,0.,0.); vec3 p = frag_pos;\n"
  "  if(p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,1.)"
  "                       &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/4.)) {\n"
  "    color.g = 0.7; } else { color.g = 0.5; }\n"
  "  if(p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,1.1)"
  "                       &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/pow(2./1.1,2))/1.1) {\n"
  "    color.g = 0.9-ceil(mod(p.x,1.)-0.5)*0.1; }\n" // TODO: make entire section above nicer.
  "  gl_FragColor = vec4(color.rgb*brightness+0.1,1.); }\0";

GLuint create_program(const GLchar *vsh, const GLchar *fsh) { GLuint vs;
  vs = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vs,1,&vsh,NULL); glCompileShader(vs);
  
  GLint compiled = 0;
  glGetShaderiv(vs,GL_COMPILE_STATUS,&compiled);
  if(!compiled) { printf("Vertex error:\n"); GLint mlen = 0;
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
  /*glBindAttribLocation(prog,0,"position");
  glBindAttribLocation(prog,1,"norm");*/ glLinkProgram(prog);
  glDeleteShader(vs); glDeleteShader(fs); return prog; }

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
  for(int i=0;i<w->e.size();i++) { d_triangulation_2(w->e[i].vd); } //d_triangulation_2(w->e[1].vd);
  glUseProgram(default_program);
  d_square(w->p.pos.x-0.05,w->p.pos.y-0.05,w->p.pos.z-0.05,0.1); }

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

void handle_input(Matrix *model, Matrix *view, Matrix *projection) {
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

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
    *view = translate(*view,v3(0,0,0.01)); }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
    *view = translate(*view,v3(0,0,-0.01)); } }

int main() { sf::ContextSettings settings;
  settings.depthBits = 24; settings.stencilBits = 8; settings.antialiasingLevel = 0;
  settings.majorVersion = 3; settings.minorVersion = 3;
  sf::Window window(sf::VideoMode(800, 800), "hang", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true); Matrix projection = gl_init(&window);
  glewExperimental = GL_TRUE;
  glewInit();

  Matrix model = translate(id_mat(4),v3(-0.5,0,-1.0));
  Matrix view = translate(id_mat(4),v3(0,0,-1.5));
  //Matrix view = look_at(v3(0,0,-1),v3(0,0,0),v3(0,1,0));

  World *w = new World(); //w->t.push_back(triangle(0,0,v3(0.5,0.5,0)));
  w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0.65,1,0.16),0.05);

  w->e = create_entities({ einit(rigid_elastic,triangulate(hole_0,EX_STEP,EX_NSTEPS),v3(0,0,0)
                                ,EX_STEP,EX_NSTEPS,true) });
  // DONE: put all of this in new construction function.
  w->e[0].shader_id = create_program(sample_vs,sample_fs);

  mvp_set(w->e[0].shader_id,model,view,projection);
  GLint u_res = glGetUniformLocation(w->e[0].shader_id,"u_res");
  glUniform2i(u_res,window.getSize().x,window.getSize().y);

  GLuint default_program = create_program(default_vs,default_fs);
  mvp_set(default_program,model,view,projection);
  for(bool r = true;r;) {
    sf::Event e; while(window.pollEvent(e)) { if(e.type==sf::Event::Closed) { r = false; } }
    handle_input(&model,&view,&projection);
    // TODO: optimize this so it does not reset every frame.
    mvp_set(default_program,model,view,projection);
    mvp_set(w->e[0].shader_id,model,view,projection);

    Matrix model_3 = mtrunc(model);
    glUseProgram(w->e[0].shader_id);
    GLint _imod = glGetUniformLocation(w->e[0].shader_id,"u_imod");
    Matrix imod = minvert(model_3);
    glUniformMatrix4fv(_imod,1,GL_TRUE,&imod.dat[0]);
    paint(w,default_program); Projectile pp = next_state(w->p);
    /*printf("<%g,%g,%g>\n",w->p.pos.x,w->p.pos.y,w->p.pos.z);*/ entity_collide(w,pp);
    window.display(); } glDeleteVertexArrays(1,&w->e[0].vd.vao); return 0; }
