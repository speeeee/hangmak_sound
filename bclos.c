/* "OS" based off of binary combinatory logic */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define COMB 0
#define PUSH 1
#define DROP 2

#define CIB 4 // combinators in byte

// len: amount of combinators or (bits used)/2.
typedef struct { uint8_t *sk; int len; } Combinator;
typedef struct { Combinator args[3]; } Args;
Args args(uint8_t *x, int asz, uint8_t *b, int bsz, uint8_t *c, int csz) { Args a;
  a.args[0].sk = malloc(asz*sizeof(uint8_t)); a.args[1].sk = malloc(bsz*sizeof(uint8_t));
  a.args[2].sk = malloc(csz*sizeof(uint8_t)); memcpy(a.args[0].sk,x,asz);
  memcpy(a.args[1].sk,b,bsz); memcpy(a.args[2].sk,c,csz);

  for(int i=1;i<3;i+=2) { a.args[i].len = i==1?asz:i==3?bsz:csz; } return a; }
void free_args(Args a) { for(int i=0;i<6;i++) { free(a.args[i].sk); } }
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
//uint32_t *comcat(Combinator x, Combinator y, Combinator z) {

// TODO: fix, most likely segfaults.
uint8_t *comcat(int csz, Combinator a, ...) { va_list vl; va_start(vl,a); int sz;
  uint8_t *n = malloc((sz = a.len)*sizeof(uint8_t)); memcpy(n,a.sk,a.len*sizeof(uint8_t));
  uint8_t *nb = n;
  for(int i=0;i<csz-1;i++) { Combinator q = va_arg(vl,Combinator);
    nb = realloc(nb,(sz += q.len)*sizeof(uint8_t)); memcpy(&n[a.len],q.sk,sz*sizeof(uint8_t));
    n+=q.len; } return n; }

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
// TODO: make Combinator recursive.
Combinator ski_eval_c(Combinator a, Stk **stk) { ski_eval(a.sk,a.len,stk); }
Combinator ski_eval(uint8_t *expr, uint32_t len, Stk **stk) { switch(expr[0]) {
  case 0: { if(len<3) { push(item(expr,len),stk); return (Combinator) { NULL, 0 }; }
    // WARNING: This will crash.
    Args a = get_args(2,&expr[1],len-1);
    Combinator l = ski_eval_c(a.args[0],stk); return l; }
  case 64: { if(len<4) { push(item(expr,len),stk); return (Combinator) { NULL, 0 }; }
    Args a = get_args(3,&expr[1],len-1);
    Combinator x = ski_eval_c(a.args[0],stk);
    Combinator y = ski_eval_c(a.args[1],stk); if(y.sk[0]<128) { y.sk[0] += 128; }
    Combinator z = ski_eval_c(a.args[2],stk);
    return (Combinator) { comcat(4,x,z,y,z), a.args[0].len+a.args[1].len+a.args[2].len }; } } }

// TODO: make conversion back to binary
// TODO: push to stack everything including SKI expressions.

int main(int argc, char **argv) { return 0; }
