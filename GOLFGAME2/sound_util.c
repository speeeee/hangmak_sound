#include <stdlib.h>
#include <stdio.h>
#include <portaudio.h>
#include <math.h>

#include "glob_structs.h"

#define SAMPLE_RATE (44100)
#define TRUMPET0 0

float ftop(float freq) { return (float)SAMPLE_RATE/freq; }

int active(int i, GState g) { return g.evs[i].act; }
void schedule(int i, GState *g) { g->evs[i].act = 1; }

float saw(int t, float p) { return 2*((float)t/p-floor(1./2+(float)t/p)); }
//  where p = SAMPLE_RATE/frequency

float TRUMPET_I[44100];
// simple trumpet simulation; linear ADSR.
float trumpet(GState *g, int att, int dec, float perc, float p) { float n = 0;
    if(active(TRUMPET0,*g)) { float nm = saw(g->evs[TRUMPET0].t,p);
      if(g->lk.x||g->lk.y) {
        if(g->evs[TRUMPET0].t<att) { n = nm*(float)(g->evs[TRUMPET0].t)/att; }
        else if(g->evs[TRUMPET0].t<att+dec) { n = nm*(dec-perc*(g->evs[TRUMPET0].t-att))/dec; }
        else { n = nm*perc; } g->evs[TRUMPET0].t++; }
      else { if(g->evs[TRUMPET0].t>44100) { g->evs[TRUMPET0].t=44100; }
        if(g->evs[TRUMPET0].t>0) { n = nm*(float)(g->evs[TRUMPET0].t--)/44100; }
        else { g->evs[TRUMPET0].act = 0; } } } return n; }
