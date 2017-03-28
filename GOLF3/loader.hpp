#include <SFML/Graphics.hpp>

#include "world.hpp"
#include "physics.hpp"
#include "polygons.hpp"
#include "grass.hpp"
//#include "general_data.hpp"

#define EX_STEP (0.02)
#define EX_NSTEPS (250)

GLuint create_program(const GLchar *, const GLchar *);
void load(World *, int);
void unload(World *, int);

/* ==== default ball shader ==== */
// TODO: rewrite for circle drawing.  Translate circle based off of given uniform position vector.
//     : rotate to always face camera.  Shade based off of global light.
static const GLchar *default_vs = "#version 330\n"
  "layout (location = 0) in vec3 position;\n"
  "layout (location = 1) in vec3 norm;\n"
  "uniform vec3 pos;\n" // displacement from origin.
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection;\n"
  "void main() { vec3 npos = position;\n"
  "  gl_Position = projection*view*model*vec4(npos.xyz+pos,1.); }\0";
static const GLchar *default_fs = "#version 330\n"
  "void main() { gl_FragColor = vec4(0.,1.,0.9,1.); }\0";

/* ==== hole_0 ==== */

// DONE: put MVP and inverse-P in fragment shader for computing eye-position.
// NOTE: in glVertexAttribPointer: first argument is the attribute (set position = 0).
static const GLchar *sample_vs = "#version 330\n"
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
// NOPE: add grass effect (perlin noise?).
static const GLchar *sample_fs = "#version 330\n"
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
  "  vec3 light = normalize(vec3(0.,-1.,-.05));\n" // example light (0,-1,0)
  "  float brightness = dot(-light,frag_norm);\n"
  "  vec3 color = vec3(0.,0.,0.); vec3 p = frag_pos;\n"
  "  if(p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,1.)"
  "                       &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/4.)) {\n"
  "    color.g = 0.7; } else { color.g = 0.5; }\n"
  "  if(p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,1.1)"
  "                       &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/pow(2./1.1,2))/1.1) {\n"
  "    color.g = 0.9-ceil(mod(p.x,1.)-0.5)*0.1; }\n" // TODO: make entire section above nicer.
  "  gl_FragColor = vec4(color.rgb*brightness+0.2,1.); }\0";

// TODO: add shading (1) and animation wrt wind (2).
static const GLchar *grass_vs = "#version 330\n"
  "layout (location = 0) in vec3 position; out vec3 frag_pos;\n"
  "layout (location = 1) in vec3 norm; out vec3 frag_norm; out vec3 frag_itra;\n"
  "uniform int utime;\n"
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection; uniform int u_grass_id;\n"
  "out mat4 frag_model; float pi = 3.14159265358979;\n"
  "float hole_0(float x, float z) { return 1./(2.*(1.+exp(-5.*(-(z-3.+pow(x-3.8,2.)/2.)))))\n"
  "                                        +sin(4.*x)/8+cos(3.*x)/12+sin(3.*z)/8.\n"
  "                                        +cos(5.*z)/12; }\n"
  "int n = 50; float interval = 0.02;\n"
  "float random_disp_2(int instance_id) {"
  "  return fract(sin(100000.*instance_id))*100000.; }\n"
  "float random_disp(int instance_id, int grass_id) {\n"
  "  return mod(int(random_disp_2(instance_id))^u_grass_id,5); }\n"
  "float curve_0(float x, float isz) {\n"
  "  return (sqrt(1-pow(x,2.)/pow(2./isz,2.))+2/(1+exp(-6.*isz*x)))/(isz*2.); }\n"
  "bool in_curve(vec3 p, float mag) {\n"
  "  return p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,mag)"
  "                           &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/pow(2./mag,2.))/mag; }\n"
  "float wind_react_y(float v, float scale) {\n"
  "  return scale*(sin(v)+cos(v/2.)+sin(v/2.-pi/4.)); }\n"
  "float wind_react_xz(float v, float scale) {\n"
  "  return scale*(cos(v)+sin(v/2.)+cos(v/2.+pi/4.)); }\n"
  // change n and interval to change amount of grass blades.
  //   n: amount of grass blades per row, amount of grass blades per column.
  //   interval: amount of space between each blade.
  // position: position in un-translated grass blade.
  "void main() {\n"
  "  frag_pos = position; frag_norm = norm;\n"
  "  vec3 p = position;\n"
  "  vec3 itra = vec3(mod(gl_InstanceID,n)*interval*5+random_disp(gl_InstanceID,u_grass_id)*interval"
  "                  ,0,gl_InstanceID/n*interval); frag_itra = itra;\n"
  "  if(in_curve(itra,1.1)) { p.y = p.y*0.1; }\n"
  "  else if(in_curve(itra,1.)) { p = p*0.5; }\n"
  "  vec3 npos = p+itra;\n"
  "  npos.y = p.y+hole_0(npos.x,npos.z);\n"
  // example of wind-animation:
  "  vec3 wind = vec3(1.0,0.0,0.0);\n"
  "  npos += wind*p.y*0.25*wind_react_xz(utime/30.+itra.x,1.);\n" // scale=10. looks interesting.
  "  npos.y += length(wind)*p.y*0.25*wind_react_y(utime/30.+itra.x,1.);\n"
  "  gl_Position = projection*view*model*vec4(npos.xyz,1.0); }\0";
static const GLchar *grass_fs = "#version 330\n"
  "in mat4 frag_model; in vec3 frag_pos; in vec3 frag_norm; in vec3 frag_itra;\n"
  "float curve_0(float x, float isz) {\n"
  "  return (sqrt(1-pow(x,2.)/pow(2./isz,2.))+2/(1+exp(-6.*isz*x)))/(isz*2.); }\n"
  "bool in_curve(vec3 p, float mag) {\n"
  "  return p.x>=0.5&&p.x<=4.5&&4.-p.z<=curve_0(p.x-2.5,mag)"
  "                           &&4.-p.z>=-sqrt(1.-pow(p.x-2.5,2.)/pow(2./mag,2.))/mag; }\n"
  "void main() {\n"
  "  vec3 color = vec3(0.,0.5,0.);\n"
  "  if(in_curve(frag_itra,1.1)) { color.g = 0.9-ceil(mod(frag_itra.x,1.)-0.5)*0.1; }\n"
  "  else if(in_curve(frag_itra,1.)) { color.g = 0.7; }\n"
  "  gl_FragColor = vec4(color.xyz+frag_pos.y*5.,1.); }\0";

// ==== hole_1 ==== //
static const GLchar *tree_vs = "#version 330\n"
  "layout (location = 0) in vec3 position; out vec3 frag_pos;\n"
  "layout (location = 1) in vec3 norm; out vec3 frag_norm;\n"
  "uniform mat4 model; uniform mat4 view; uniform mat4 projection; uniform vec3 col;\n"
  "out mat4 frag_model; out vec3 frag_col;\n"
  "float samp_func(vec2 v) { return pow(v.x,2.); }\n"
  "void main() {\n"
  "  frag_pos = position; frag_norm = norm; frag_col = col;\n"
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
// NOPE: add grass effect (perlin noise?).
static const GLchar *tree_fs = "#version 330\n"
  "uniform ivec2 u_res;\n"
  //"uniform mat4 imvp;\n"
  "uniform mat3 u_imod;\n"
  "in mat4 frag_model;\n"
  "in vec3 frag_pos; in vec3 frag_norm; in vec3 frag_col;\n"
  /*"void main() { vec4 ndc_pos; ndc_pos.xy = (2.0*gl_FragCoord.xy)/(u_res.xy)-1;\n"
  "  ndc_pos.z = (2.0*gl_FragCoord.z-gl_DepthRange.near-gl_DepthRange.far) /"
  "              (gl_DepthRange.far - gl_DepthRange.near);\n"
  "  ndc_pos.w = 1.0; vec4 clip_pos = ndc_pos / gl_FragCoord.w;\n"
  "  vec4 pos = imvp*clip_pos;\n"*/
  // as isz grows, the curve gets smaller.
  "float curve_0(float x, float isz) {\n"
  "  return (sqrt(1-pow(x,2.)/pow(2./isz,2.))+2/(1+exp(-6.*isz*x)))/(isz*2.); }\n"
  "void main() {\n"
  "  vec3 light = normalize(vec3(0.,-1.,-1.));\n" // example light (0,-1,0)
  "  float brightness = dot(-light,frag_norm);\n"
  "  vec3 color = frag_col; vec3 p = frag_pos;\n"
  "  gl_FragColor = vec4(color.rgb+(brightness-0.5)*0.25,1.); }\0";
