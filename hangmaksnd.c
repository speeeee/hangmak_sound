//========================================================================
// UTF-8 window title test
// Copyright (c) Camilla Berglund <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//
// This test sets a UTF-8 window title
//
//========================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "portaudio.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define SAMPLE_RATE (44100)
#define FPB (64)

// both of these are directly related to `getInput'.
// KC for Key Count is the amount of keys read.
#define KC (4)
// RC for Result Count is how many results are expected from `getInput'. 
#define RC (2)

#define SILENT  0
#define PLAYING 1

typedef struct { GLfloat x; GLfloat y; } Player;
typedef struct { GLfloat x; GLfloat y; } KState;

typedef struct { int sz; float *samp; int lp; int rp; } Snd;
typedef struct { KState lk; int keys[KC]; } Keys;
typedef struct { float time; Snd s; int stat; } Sched;
typedef struct Queue { Sched sc; struct Queue *n; } Queue;
typedef struct { Player pl; } GState;
//data dat; PaStream *stream;
//PaStreamParameters oP;

// more of a priority queue.
Queue *push_q(Sched sc, Queue *o) { Queue *n = malloc(sizeof(Queue));
  n->sc = sc; n->n = NULL; if(!o) { return n; }
  else { Queue *i = o; for(;i&&sc.time > i->sc.time;i = i->n); i->n = n; return o; } }
Queue *drop_q(Queue *o) { if(!o) { printf("ERROR: queue is empty.\n"); }
  else { Queue *a = o->n; /*free(a->sc.s.samp);*/ free(a); return o->n; } }
Queue *get_q(int a, Queue *q) { for(;a>0&&q;q=q->n,a--); return q; }

void setup(GLFWwindow *win) {
  float ratio; int width, height; glfwGetFramebufferSize(win, &width, &height);
  ratio = width / (float) height; glViewport(0,0,width,height);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); glOrtho(0,1,0,1,0,1); glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); }

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description); }

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height); }

/*void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  if(key == GLFW_KEY_A && action == GLFW_PRESS) {
    Pa_StartStream(stream); } else { Pa_StopStream(stream); } }*/

/*static int pCallBack(const void *inputBuffer, void *outputBuffer, unsigned long fpb,
                     const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                     void *userData) {
  data *d = (data *)userData;
  float *out = (float *)outputBuffer;
  unsigned long i;

  for(i=0;i<fpb;i++) { *out++ = d->sine[d->lp]; *out++ = d->sine[d->rp];
    d->lp += 1; d->rp += 1;
    if(d->lp >= 44100) { d->lp -= 44100; }
    if(d->rp >= 44100) { d->rp -= 44100; } } return paContinue; }*/

int any_eq(int a, int *b, int fro, int to) { int i; for(int i=fro;i<to&&b[i]!=a;i++);
  return i>=to?-1:i; }

int get_amt_playing(Queue *q) { int i; for(i=0;q&&q->sc.stat;i++,q=q->n); return i; }
int play_s(Queue *q, float t) { int i;
  for(i=0;q&&t>=q->sc.time&&!q->sc.stat;q=q->n,i++) { q->sc.stat = PLAYING; } return i; }

void out_q(float *out, Queue **d) { *out += (*d)->sc.s.samp[(*d)->sc.s.lp++];
  *(out+1) += (*d)->sc.s.samp[(*d)->sc.s.rp++]; }
  //if((*d)->sc.s.lp >= (*d)->sc.s.sz || (*d)->sc.s.rp >= (*d)->sc.s.sz) {
  //  *d = drop_q(*d); } }

// This callback will terminate when the supplied sample ends.
int detCallback(const void *in, void *outputBuffer, unsigned long fpb,
                const PaStreamCallbackTimeInfo *tInfo, PaStreamCallbackFlags statFlags,
                void *userData) {
  Queue *d = (Queue *)userData; float *out = (float *)outputBuffer;
  int amt_p = get_amt_playing(d); amt_p += play_s(get_q(amt_p,d),tInfo->currentTime);

  for(int i=0;i<fpb;i++) { out[0] = 0; out[1] = 0;
    for(int e=0;e<amt_p;e++,d=d->n) { out_q(out,&d); } *out++; *out++; }
    //*out++ = d->samp[d->lp++]; *out++ = d->samp[d->rp++];
    // the stream can be cut prematurely outside, and the phase values will be left in their
    //   previous state, though will be reset when calling the endStream function, which picks
    //   up where this left off.
    //if(d->lp >= d->sz || d->rp >= d->sz) { d->lp = d->rp = 0; return paComplete; } }
  return paContinue; }

Snd snd_copy(Snd a) { float *d = malloc(a.sz*sizeof(float));
  memcpy(d,a.samp,a.sz*sizeof(float)); return (Snd) { a.sz, d, a.lp, a.rp }; }
void to_down(Snd *a) { Snd n; n.lp = n.rp = 0; n.sz = a->sz;
  n.samp = malloc(n.sz*sizeof(float));
  for(int i=0;i<n.sz;i++) { n.samp[i] = a->samp[i]; } free(a->samp); *a = n; }
void in_snd(Snd *a, Snd b) { Snd c = snd_copy(b); free(a->samp); *a = c; }

void nstr(Snd snd, PaStreamParameters oP, PaStream *stream) {
  Pa_OpenStream(&stream,NULL,&oP,SAMPLE_RATE,FPB,paClipOff,detCallback,&snd); }
void psound_det(PaStream *stream) {
  if(Pa_IsStreamStopped(stream)) { Pa_StartStream(stream); } }

void paint(GLFWwindow *win, GState g) { glColor3f(1.0,0.0,0.0);
  glBegin(GL_QUADS); glVertex3f(g.pl.x,g.pl.y,0); glVertex3f(g.pl.x+0.1,g.pl.y,0);
                     glVertex3f(g.pl.x+0.1,g.pl.y+0.1,0); glVertex3f(g.pl.x,g.pl.y+0.1,0);
  glEnd(); }

int pressed(GLFWwindow *win, int k) { return glfwGetKey(win,k)==GLFW_PRESS; }

//void set_k(GLfloat a[RC], GLfloat b[RC]) { for(int i=0;i<RC;i++) { a[i] = b[i]; } }
// all key processing happens here.
KState getInput(GLFWwindow *win) { KState n;
  n.x = pressed(win,GLFW_KEY_D)-pressed(win,GLFW_KEY_A);
  n.y = pressed(win,GLFW_KEY_W)-pressed(win,GLFW_KEY_S); return n; }

/*int *getKeys(int keys[KC], GLFWwindow *win) { int a[KC];
  for(int i=0;i<ksz;i++) { a[i] = glfwGetKey(win,keys[i]); } return a; }*/
void procInput(GState *g, KState *lk, GLFWwindow *win) { KState a =  getInput(win);
  g->pl.x += a.x*0.01; g->pl.y += a.y*0.01; *lk = a; }

int main(void) {
    GState g = (GState) { (Player) { 0, 0 } }; KState lk = { 0, 0 };
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(400, 400, "hangmak_sound", NULL, NULL);
    if(!window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT); paint(window,g); 
      procInput(&g,&lk,window); glfwSwapBuffers(window);
      glfwPollEvents(); }
    //error:
    //Pa_StopStream(stream); Pa_CloseStream(stream); Pa_Terminate();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

