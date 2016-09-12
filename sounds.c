#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "ft.h"

// change if desired.
#define SAMPLE_RATE 44100

typedef struct { int *data; int sz; } Snd;
typedef struct { int a; int exists; } MaybeI;
MaybeI maybeI(int a, int exists) { return (MaybeI) { a, exists }; }

typedef int (*IFun)(int, int);

// variable __IND defined for every callback call.  Used only for bounds-checking.

// the callback stops once Nothing has been found during the call.

// Time -> Double -> Snd -> MaybeI Int
#define pitch(T,D,S) \
  (__IND = ((double)(T)*(D)))>(S).sz? \
    maybeI(0, 0):maybeI((S).data[(int)__IND], 1)
int square(int a, int p) { return p * ((a>0)-(a<0)); }

int add(int a, int b) { a+b; }
int sub(int a, int b) { a-b; }
int mul(int a, int b) { a*b; }
int quo(int a, int b) { a/b; }

MaybeI fmap(MaybeI a, MaybeI b, IFun f) {
  if(!(a.exists&&b.exists)) { return maybeI(0,0); } return maybeI((f)(a.a,b.a),1); }

SNDFILE *wav_snd(const char *n, int sz, int nchan, int sfm) { 
  SF_INFO sf; memset(&sf, 0, sizeof(sf)); SNDFILE *in;
  sf.samplerate = SAMPLE_RATE;  sf.frames = sz; sf.channels = nchan;
  sf.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
  if(!(in = sf_open(n, sfm, &sf))) { printf("no\n"); exit(0); }
  return in; }
Snd in_f(const char *n, int sz, int nchan) { int *buffer = malloc(sz*sizeof(int));
  SNDFILE *in = wav_snd(n,sz,nchan,SFM_READ); sf_read_int(in,buffer,sz); sf_close(in);
  return (Snd) { buffer, sz }; }

int peak(Snd a) { int p=0; for(int i=0;i<a.sz;i++) { if(abs(a.data[i])>p) { p=a.data[i]; } }
  return abs(p); }

void lpf(Snd a, double mul) { for(int i=1;i<a.sz;i++) {
  a.data[i] = (int)((double)(a.data[i]+a.data[i-1])/2); } }

void amp(Snd a, float perc) { for(int i=0;i<a.sz;i++) { a.data[i] = (int)a.data[i]*perc; } }

// context-based functions (pitch) vs linear functions (add)
/* context-based functions require buffers to work due to needing context.  All context-based
     functions return MaybeI.  Linear functions need only the items they are given.
     All linear functions return int (and can be used with fmap for MaybeI). */
int main(int argc, char **argv) { Snd key = in_f("sine.wav",22050,1);
  SNDFILE *out = wav_snd("target.wav",44100,1,SFM_WRITE); MaybeI res = maybeI(0,0);
  double __IND = 0; int p = peak(key);
  //for(int t=0;(res = pitch(t,2,key)).exists;t++) { sf_write_int(out,&res.a,1); }
  for(int t=0;t<key.sz&&(res = maybeI(square(key.data[t],p),1)).exists;t++) {
    sf_write_int(out,&res.a,1); }
  sf_close(out); return 0; }
