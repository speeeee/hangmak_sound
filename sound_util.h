#include <stdlib.h>
#include <stdio.h>
#include <portaudio.h>
#include <math.h>

#include "glob_structs.h"

float ftop(float freq);

int active(int i, GState g);
void schedule(int i, GState *g);

float saw(int t, float p);
//  where p = SAMPLE_RATE/frequency

float TRUMPET_I[44100];
// simple trumpet simulation; linear ADSR.
float trumpet(GState *g, int att, int dec, float perc, float p);
