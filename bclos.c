/* "OS" based off of binary combinatory logic */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define COMB 0
#define PUSH 1
#define DROP 2

#define CIB 4 // combinators in byte

// len: amount of combinators or (bits used)/2.
typedef struct { uint8_t *sk; int len; } Combinator;
typedef struct { uint8_t *args[6]; } Args;
Args args(uint8_t *x, int asz, uint8_t *b, int bsz, uint8_t *c, int csz) { Args a;
  a.args[0] = malloc(asz*sizeof(uint8_t)); a.args[2] = malloc(bsz*sizeof(uint8_t));
  a.args[4] = malloc(csz*sizeof(uint8_t)); memcpy(a.args[0],x,asz);
  memcpy(a.args[2],b,bsz); memcpy(a.args[4],c,csz);

  for(int i=1;i<7;i+=2) { a.args[i] = malloc(sizeof(uint32_t));
    memcpy(a.args[i],i==1?&asz:i==3?&bsz:&csz,sizeof(uint32_t)); } return a; }
void free_args(Args a) { for(int i=0;i<6;i++) { free(a.args[i]); } }
typedef struct { uint8_t *x; int sz; } Item;
Item item(uint8_t *x, int sz) { return (Item) { x, sz }; }
typedef struct Stk { Item i; struct Stk *p; } Stk;

void push(Item a, Stk **stk) { Stk *e = malloc(sizeof(Stk));
  e->i = a; if(*stk) { e->p = *stk; } else { e->p = NULL; } *stk = e; }
Item pop(Stk **stk) { if(*stk) { free((*stk)->i.x); Stk *e = *stk; *stk = (*stk)->p;
  Item a = e->i; free(e); return a; } return item(NULL,0); }

Item item_from_f(FILE *f, uint64_t amt, uint64_t loc) { size_t prev = ftell(f);
  fseek(f,SEEK_SET,loc); uint8_t *a; fread(a,amt,1,f); fseek(f,SEEK_SET,prev);
  return item(a,amt); }

int argsz(int amt, uint8_t *expr, int len) { int i = 0;
  for(;i<len&&amt>0;i++) {
    if(expr[i]==192||expr[i]==128) { i += argsz(expr[i]==192?3:2,&expr[i],len-i)-1; }
    // if wanted, using the fact that 192%5=2 and 128%5=3, expr[i]==192?3:2 can be shortened.
    amt--; } return i; }
Args get_args(int amt, uint8_t *expr, int len) { int x = argsz(amt,expr,len);
  int y = argsz(amt,&expr[x],len-x); int z = 0;
  if(amt==3) { z = argsz(amt,&expr[y+x],len-x-y); }
  return args(expr,x,&expr[x],y,NULL,0); }

Combinator ski_eval(uint8_t *, uint32_t, Stk **);

uint8_t *ski(Combinator exp, Stk **stk) {
  // S = 01; K = 00; (S X Y Z) = 11; (K X Y) = 10; I = SK
  // rewritten: S = 64; K = 0; (S X Y Z) = 192; (K X Y) = 128; I = SK
  // NOTE: SKKx -> Kx(Kx) -> x
  int len = exp.len; uint8_t *lst = malloc(len*2*sizeof(uint8_t));
  for(int q=0;q<len/CIB+len%CIB>0;q++) { 
    for(int i=0;i<CIB-len%CIB*CIB/len;i++) {
      lst[i+q*CIB] = exp.sk[q] << (uint8_t)i & (uint8_t)192; } }
  return ski_eval(lst,len,stk).sk; }
Combinator ski_eval(uint8_t *expr, uint32_t len, Stk **stk) { switch(expr[0]) {
  case 0: { if(len<3) { push(item(expr,len),stk); return (Combinator) { NULL, 0 }; }
    // WARNING: This will crash.
    Args a = get_args(2,&expr[1],len-1);
    Combinator l = ski_eval(a.args[0],*(uint32_t *)a.args[1],stk); return l; }
  case 64: { if(len<4) { push(item(expr,len),stk); return (Combinator) { NULL, 0 }; }
    /* TODO: write S combinator */ return (Combinator) { NULL, 0 }; } } }

// TODO: make conversion back to binary
// TODO: push to stack everything including SKI expressions.

int main(int argc, char **argv) { return 0; }
