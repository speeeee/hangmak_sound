#include <glad/glad.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "portaudio.h"
#include <math.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "glob_structs.h"

#include "gl_util.h"
#include "sound_util.h"
#include "stage_util.h"
#include "func_util.h"
#include "physics.h"

#define SAMPLE_RATE (44100)
#define FPB (64)

// both of these are directly related to `getInput'.
// KC for Key Count is the amount of keys read.
#define KC (4)
// RC for Result Count is how many results are expected from `getInput'. 
#define RC (2)

#define SILENT  0
#define PLAYING 1

#define TRUMPET0 0

#define GRAVITY (-0.0005)

#define P_VEC v3(g.pl.x,g.pl.y,g.pl.z)
#define P_VEC_PT v3(g->pl.x,g->pl.y,g->pl.z)

typedef int (*TFun)(int, ...);

// TODO: rename vect3 to pla3.
Player vect3(GLfloat x, GLfloat y, GLfloat z) {
  return (Player) { x, y, z, (Vec3) { 0, 0, 0 }, (Vec3) { 0, 0, 0 } }; }

// == Instruments =================== //
Instr instr(int t, int act) { return (Instr) { t, act }; }
void g_add_instr(Instr **a, int esz, int sz, ...) { va_list vl; va_start(vl,sz);
  if(*a) { *a = malloc(sz*sizeof(Instr)); } else { *a = realloc(*a,(esz+sz)*sizeof(Instr)); }
  for(int i=esz;i<esz+sz;i++) { *a[i] = va_arg(vl,Instr); }
  va_end(vl); }

// ================================== //


// == global variables here are strictly for testing ========== //

PLANE(45_tilt,v3(pow(2.,0.5),pow(2.,0.5),0))
FUNCTION_JUST_X(sine,sin)

// func_sin5
GLfloat mul_5(GLfloat x) { return 5*x; }
GEN_FUNCTION(sin5,COMPOSE(sin,COMPOSE_X(mul_5)))

// assume that y does not change with z.
GLfloat deriv_sin5_x(GLfloat x, GLfloat z) { return 5*cos(5*x); }
// gradient
Vec2 deriv_sin5_xg(GLfloat x, GLfloat z) { return v2(5*cos(5*x),0); }

GLfloat x_squared(GLfloat x, GLfloat z) { return pow(x,2.); }
Surface s;

// ============================================================ //

typedef int (*Pred)(GState);
//data dat; PaStream *stream;
//PaStreamParameters oP;

/*const char *ver_v = "uniform float x; uniform float y; void main(void) { vec4 v = vec4(gl_Vertex);\n"
  "vec4 col = vec4(gl_Color); float dist = pow(pow(x+v.x,2.0)+pow(y+v.y,2.0),0.5);"
  "vec4 nc = vec4(dist/col.x*5.0,dist/col.y*5.0,0.0,1.0);"
  "gl_Position = gl_ModelViewProjectionMatrix * v;"
  "gl_FrontColor = nc; }";*/
// use glUniform3fv
const char *ver_v =
  "void main(void) { vec4 v = vec4(gl_Vertex);"
  "gl_FrontColor = gl_Color; gl_Position = gl_ModelViewProjectionMatrix*v; }";
const char *frag_v = "void main(void) { vec4 v = vec4(gl_Color);"
  "gl_FragColor = v; }";

static GLuint mk_shader(GLenum type, const char *src) {
  GLuint ok; GLsizei ll; char info_log[8192];
  GLuint shader = glCreateShader(type); glShaderSource(shader, 1, (const GLchar **)&src, NULL);
  glCompileShader(shader); glGetShaderiv(shader, GL_COMPILE_STATUS, &ok); 
  if(ok != GL_TRUE) { glGetShaderInfoLog(shader, 8192, &ll, info_log);
    fprintf(stderr, "ERROR: \n%s\n\n", info_log); glDeleteShader(shader); shader = 0u; }
  return shader; }
static GLuint mk_shader_program(const char *vs_src, const char *fs_src) { GLuint prog = 0u;
  GLuint vs = 0u; GLuint fs = 0u; vs = mk_shader(GL_VERTEX_SHADER, vs_src);
  fs = mk_shader(GL_FRAGMENT_SHADER, fs_src); prog = glCreateProgram();
  glAttachShader(prog,vs); glAttachShader(prog,fs); glLinkProgram(prog); return prog; }

#define warray_(N,X,F,...) GLfloat *N = X; (F)(__VA_ARGS__,X); free(N);
// where (GLfloat *X), ((GLFloat * -> void) *F), ...
// more of a priority queue.

Queue *drop_q(Queue *o) { if(!o) { printf("ERROR: queue is empty.\n"); }
  else { Queue *a = o->n; /*free(a->sc.s.samp);*/ free(a); return o->n; } }
Queue *get_q(int a, Queue *q) { for(;a>0&&q;q=q->n,a--); return q; }

// remember to free.
// WARNING: arguments passed to the function MUST be GLdoubles.
GLfloat *farr(int sz, ...) { va_list vl; va_start(vl,sz); GLfloat *a = malloc(sz*sizeof(GLfloat));
  for(int i=0;i<sz;i++) { a[i] = (GLfloat)va_arg(vl,GLdouble); } va_end(vl); return a; }

void perspective(GLdouble fovy, GLdouble asp, GLdouble znear, GLdouble zfar) {
  const GLdouble pi = 3.14159265358979323846264338327;
  GLdouble top = znear*tan(pi/360.*fovy);
  glFrustum(-top*asp,top*asp,-top,top,znear,zfar); }

/*void init_gl(GLFWwindow *win) {
  float ratio; int width, height; glfwGetFramebufferSize(win, &width, &height);
  ratio = width / (float) height; glViewport(0,0,width,height);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); glOrtho(0,1,0,1,0,1); glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); }*/
void rsz(GLFWwindow *win, int w, int h) { glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  perspective(45,w/(float)h,0.1,100); glMatrixMode(GL_MODELVIEW); glLoadIdentity();
  glFlush(); }
void init_gl(GLFWwindow *win) { glShadeModel(GL_SMOOTH);
  glClearColor(0,0,0,0); glClearDepth(1); glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); int w, h; glfwGetFramebufferSize(win,&w,&h);
  rsz(win,w,h); }

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description); }

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height); }

int any_eq(int a, int *b, int fro, int to) { int i; for(int i=fro;i<to&&b[i]!=a;i++);
  return i>=to?-1:i; }

/* == Sound synthesis ======== */

void init_instrs(void) { for(int t=0;t<44100;t++) { TRUMPET_I[t] = saw(t,ftop(440.0)); } }
// This callback will terminate when the supplied sample ends.
int detCallback(const void *in, void *outputBuffer, unsigned long fpb,
                const PaStreamCallbackTimeInfo *tInfo, PaStreamCallbackFlags statFlags,
                void *userData) {
  GState *g = (GState *)userData; float *out = (float *)outputBuffer;
  int t = g->t;
  for(;t<g->t+64;t++) { float n = 0;
    if((g->lk.x||g->lk.y)&&!active(TRUMPET0,*g)&&0) { schedule(TRUMPET0,g); }
    //float n = /*sin(440*2*M_PI*((double)t/(double)SAMPLE_RATE));*/
              //trumpet(t,ftop(440.0)); int tr = t%44100;
              //if(tr<8820) { n *= tr/8820; } *out++ = n; *out++ = n; }
    n = trumpet(g,882,882,0.75,ftop(440.));
    *out++ = n; *out++ = n; }
    // the stream can be cut prematurely outside, and the phase values will be left in their
    //   previous state, though will be reset when calling the endStream function, which picks
    //   up where this left off.
    //if(d->lp >= d->sz || d->rp >= d->sz) { d->lp = d->rp = 0; return paComplete; } }
  g->t = t; return paContinue; }

void psound_det(PaStream *stream) {
  if(Pa_IsStreamStopped(stream)) { Pa_StartStream(stream); } }

//GLfloat deg_rad(GLfloat a) { return a*M_PI/180; }

void paint(GLFWwindow *win, GLuint prog, GState g) { glLoadIdentity();
  glTranslatef(0,0,/*-1.5*/-5);
  glRotatef(g.ca.cxz,0,-cos(deg_rad(g.ca.cyz)),-sin(deg_rad(g.ca.cyz))); glRotatef(g.ca.cyz,1,0,0);

  glColor4f(1.0,0.0,0.0,1.0);
  //GLfloat pos = glGetUniformLocation(prog,"x"); glUniform1f(pos,g.pl.x);
  //GLfloat posy = glGetUniformLocation(prog,"y"); glUniform1f(posy,g.pl.z);
  //cube(pt(-0.05,0,-0.05),0.1);
  cube(pt(g.pl.x-0.05,g.pl.y,g.pl.z-0.05),0.1);
  glBegin(GL_QUADS);
    //glVertex3f(-0.05,0,0.05); glVertex3f(0.05,0,0.05);
    //glVertex3f(0.05,0.1,0.05); glVertex3f(-0.05,0.1,0.05);

    // TODO: inspect possible issue with rendering.
    glColor3f(0.2989,0.5,0.411);
    //glVertex3f(-3-g.pl.x,-g.pl.y,-3+g.pl.z); glVertex3f(3-g.pl.x,-g.pl.y,-3+g.pl.z);
    //glVertex3f(3-g.pl.x,-g.pl.y,3+g.pl.z); glVertex3f(-3-g.pl.x,-g.pl.y,3+g.pl.z);
    render_surface(s,0.1);
  glEnd(); }

int pressed(GLFWwindow *win, int k) { return glfwGetKey(win,k)==GLFW_PRESS; }

//void set_k(GLfloat a[RC], GLfloat b[RC]) { for(int i=0;i<RC;i++) { a[i] = b[i]; } }
// all key processing happens here.
KState getInput(GLFWwindow *win) { KState n;
  n.x = pressed(win,GLFW_KEY_D)-pressed(win,GLFW_KEY_A);
  n.z = pressed(win,GLFW_KEY_W)-pressed(win,GLFW_KEY_S);
  n.y = pressed(win,GLFW_KEY_R)-pressed(win,GLFW_KEY_F); 
  n.tht = pressed(win,GLFW_KEY_UP)-pressed(win,GLFW_KEY_DOWN);
  n.phi = pressed(win,GLFW_KEY_RIGHT)-pressed(win,GLFW_KEY_LEFT); return n; }

/*int *getKeys(int keys[KC], GLFWwindow *win) { int a[KC];
  for(int i=0;i<ksz;i++) { a[i] = glfwGetKey(win,keys[i]); } return a; }*/
// TODO: make into a vect3 the position.
void procInput(GState *g, GLFWwindow *win) { KState a =  getInput(win);
  GLfloat cx = sin(deg_rad(g->ca.cxz)); GLfloat cz = -cos(deg_rad(g->ca.cxz));
  
  // change acceleration and velocity
  g->pl.acc = v3(0.,g->gravity,0.); g->pl.vel = vec_add(g->pl.acc,g->pl.vel); 
  // change position
  g->pl.x += -a.z*0.01*cx-a.x*0.01*cz+g->pl.vel.x;
  g->pl.y += a.y*0.01+g->pl.vel.y; g->pl.z += -a.z*0.01*cz+a.x*0.01*cx+g->pl.vel.z;
  // change camera position
  g->ca.cxz += a.phi; g->ca.cyz += a.tht; g->lk = a; }

/* Initialize PortAudio; pass to PortAudio the GState; use function that takes the state and returns
     an output sample (for example, if the state function is to return a sine function, then
                       it will just return the sample of the sine at the GState's time).
     The function would usually have a (Predicate,SndState) array.  When sounds are to be added,
     the PortAudio stream stops and the new sound is added to the array with a given predicate.
     All predicates take a GState as their input. */
int main(void) { init_instrs(); Instr trumpet = instr(0,0); Instr *a = NULL;
    PaStreamParameters oP; PaStream *stream;
    GState g = (GState) { vect3(-M_PI/10,0.5,0), (Camera) { 0, 0 }, (KState) { 0, 0, 0, 0, 0 }, 0,
                          a, 0, GRAVITY };
    g_add_instr(&g.evs,g.esz++,1,trumpet);
    GLFWwindow* window;

    //g.pl.vel.z = 0.001;

    // == allocation of TEST globals ========== //

    //p.type = RIGID_COLLISION; p.pts = Vec3_arr(4,v3(-1,0,-1),v3(1,0,-1),v3(1,0,1),v3(-1,0,1));
    //p.normal = norm(v3(0,1,0)); // DONE: normalize

    s.bl = v3(-2,0,-2); s.tr = v3(2,0,2); s.effect_type = RIGID_COLLISION;
    s.fun = func_sin5;

    // ======================================== //

    Pa_Initialize();
    glfwSetErrorCallback(error_callback);

    oP.device = Pa_GetDefaultOutputDevice();
    oP.channelCount = 2;
    oP.sampleFormat = paFloat32;
    oP.suggestedLatency = Pa_GetDeviceInfo(oP.device)->defaultLowOutputLatency;
    oP.hostApiSpecificStreamInfo = NULL;

    Pa_OpenStream(&stream,NULL,&oP,SAMPLE_RATE,FPB,paClipOff,detCallback,&g);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(400, 400, "hangmak_sound", NULL, NULL);
    if(!window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    GLuint prog = 0u; prog = mk_shader_program(ver_v, frag_v);
    glUseProgram(prog);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    init_gl(window);
    //glfwSetKeyCallback(window, key_callback);
    psound_det(stream);
    while (!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); paint(window,prog,g);
      //if(test_collision_below(P_VEC,s)) { rigid_collision(&g,v3(
      //if(within_bounds(P_VEC,s)) { printf("in the bounds\n"); }
      procInput(&g,window);
      if(test_collision_below(P_VEC,next_position(g),s)) {
        /*rigid*//*normal*//*normal_degrade*/normal2_collision(&g,func_sin5,deriv_sin5_xg); }
        //if(g.pl.vel.y<-GRAVITY) { printf("HERE");g.pl.vel.y = 0; g.pl.y = func_sin5(g.pl.x,g.pl.z); } }
        //g.pl.vel = v3((n<0?-1:1)*0.01*cos(atan2(n,1)),(n<0?-1:1)*0.01*sin(atan2(n,1)),0); }
      //procInput(&g,window);
      glfwSwapBuffers(window);
      glfwPollEvents(); }
    error:
    Pa_StopStream(stream); Pa_CloseStream(stream); Pa_Terminate();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

