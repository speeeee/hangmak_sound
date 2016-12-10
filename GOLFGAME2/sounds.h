#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

typedef struct { int *data; int sz; } Snd;
typedef struct { int a; int exists; } MaybeI;
MaybeI maybeI(int, int);

typedef int (*IFun)(int, int);

int square(int, int);
int lpf(int, Snd, float);
int amp(int, float);

int add(int, int); int sub(int, int); int mul(int, int); int quo(int, int);

MaybeI fmap(MaybeI, MaybeI, IFun);
SNDFILE *wav_snd(const char *, int, int, int);
Snd in_f(const char *, int, int);

int peak(Snd);
