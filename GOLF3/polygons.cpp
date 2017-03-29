#include <vector>
#include <cmath>

#include "world.hpp"

int ex_bounds_2(Vec2 a) { return a.x>0&&a.x<5.0&&a.z>0&&a.z<5.0; }
float hole_0(float x, float z) { return 1./(2*(1+exp(-5*(-(z-3+pow(x-3.8,2.)/2.)))))
                                        +sin(4*x)/8+cos(3*x)/12+sin(3*z)/8
                                        +cos(5*z)/12; }
// divide into 30 circles
#define CIRCLES 30
float circ(float a, float x) { return pow(pow(a,2.)-pow(x,2.),0.5); }
float test_cyl_0(float tht, float y) { /*return 0.05;*/
  float a = 2*M_PI/CIRCLES;
  return 0.5+circ(a,fmod(tht,a)-a/2.); }

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
    // TODO: possible error in third triangle.
    for(int j=apv*3;j<nsteps*fpg;j+=apv) { int ind = j+d;
      if((j/apv)%2) { ret[ind+1] = f(j/fpg*step,i*step+step); ret[ind+2] = i*step+step; }
      else { ret[ind+1] = f(j/fpg*step,i*step); ret[ind+2] = i*step; }
      ret[ind] = j/fpg*step;

      Vec3 a = arr_to_vec(&ret[ind]); Vec3 b = arr_to_vec(&ret[ind-apv]);
      Vec3 c = arr_to_vec(&ret[ind-apv*2]);
      /*Vec3 v0 = b-a; Vec3 v1 = c-a;
      if((j/apv)%2) { v0.x = -v0.x; v0.z = -v0.z; v0.y = -v0.y;
                      v1.x = -v1.x; v1.z = -v1.z; v1.y = -v1.y; }
      else { v0.x = -v0.x; v0.y = -v0.y;
             v1.x = -v1.x; v1.y = -v1.y; }
      norm = unit(cross(v0,v1));*/
      if((j/apv)%2) { norm = norm_positive(a,b,c); } else { norm = norm_positive(c,b,a); }
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
// DONE: change to not force function-based render.
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

    std::vector<Triangle> btris;
    if(std::get<6>(ei[i])) { btris = to_triangles(tris,step); }
    asadd(pos,&tris[0],tris.size(),STRIDE); // for stride.

    // calculation for vaod.pos is dat.size()/stride
    //   (dat.size() is guaranteed to be a multiple of stride).
    ret.push_back(entity(pos,btris,std::vector<float>()
                        ,vao_dat(dat.size()/STRIDE,nsteps,std::get<5>(ei[i]),vao)
                        ,ex_bounds_2,cf,std::get<5>(ei[i]),0));
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
  glVertexAttribPointer(pos_attrib,3,GL_FLOAT,GL_FALSE,STRIDE*sizeof(GL_FLOAT),0); // reminder: first argument requires
                                                    //   glBindAttribLocation(..).
  glEnableVertexAttribArray(norm_attrib);
  glVertexAttribPointer(norm_attrib,3,GL_FLOAT,GL_FALSE,6*sizeof(GL_FLOAT)
                       ,(const GLvoid *)(3*sizeof(GL_FLOAT)));

  return ret; }

// TODO: create compositional system where certain vertices can be removed on bounds check.
//     : e.g. creating a model of penne pasta could be with small cylinders connected
//     :   to each other to form a circle and clipping any points within the circle
//     :   that crosses through the radii of the cylinders.
/* ==== produce vector from cylindrical function f(theta,y) = r ==== */

// ideally, tstep should be a factor of (ub-lb) (upper_bound-lower_bound).
// TODO: add normals.
std::vector<float> cyl_to_tris(FuncXZ cf, float tstep, float ystep, float lb, int tnsteps
                              ,int height) {
  // every step has two points for drawing, both on the same x-coordinate.
  // six total floats for each step.  this is doubled for all associated normals.
  int floats_per_vert = 3;
  // one group is, in this case, the two vertices whose line is parallel to y-axis.
  //   e.g. |||||| : two vertices per line.
  int verts_per_group = 2;
  int n_attribs       = 2;

  // attributes per vertex * floats_per_vert
  int apv = floats_per_vert*n_attribs; // = 6

  // floats_per_group
  int fpg = floats_per_vert*verts_per_group*n_attribs; // = 12.
  std::vector<float> ret(tnsteps*height*fpg);

  for(int j=0;j<height;j++) {
    for(int ip=0;ip<tnsteps;ip++) { float c = lb+(float)(ip*tstep); int i = ip*fpg;
      float r = cf(c,(float)(height*ystep)); float x = r*cos(c); float z = r*sin(c); 
      ret[i] = ret[i+6] = x; ret[i+2] = ret[i+8] = z;
      ret[i+1] = (float)(height*ystep); ret[i+7] = (float)(height*ystep*2);
      if(ip>0) {
        Vec3 a = arr_to_vec(&ret[i]); Vec3 b = arr_to_vec(&ret[i-apv]);
        Vec3 c = arr_to_vec(&ret[i-apv*2]);
        Vec3 anorm = norm_positive(a,b,c); Vec3 bnorm = norm_positive(c,b,a);
        ret[i+3] = anorm.x; ret[i+4] = anorm.y; ret[i+5] = anorm.z;
        ret[i+9] = bnorm.x; ret[i+10] = bnorm.y; ret[i+11] = bnorm.z; } } }
  return ret; }

// ignore for now.
std::vector<float> aappend(std::vector<float> a, std::vector<float> b, int stride) {
  a.insert(a.end(),b.begin(),b.end()); }

std::vector<float> c_ring(std::vector<float> a, float rad, float step, int nsteps) { std::vector<float> ret;
  for(int i=0;i<nsteps;i++) { std::vector<float> b = a;
    asadd(v3(rad*cos(step*i),0,rad*sin(step*i)),&b[0],b.size(),STRIDE);
    aappend(ret,b,STRIDE); } return ret; }

std::vector<float> c_clip(std::vector<float> a, float rad) {
  for(int i=0;i<a.size();i+=6) { if(pow(pow(a[i],2.)+pow(a[i+2],2.),0.5)>rad) {
    a.erase(a.begin()+i,a.begin()+i+5); } } return a; }
