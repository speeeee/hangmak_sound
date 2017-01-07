#include "world.hpp"
#include "bounds.hpp"

#include <memory>
// uses Projectile.

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile a) { Vec3 nv = vadd3(a.acc, a.vel);
  return (Projectile) { a.acc, nv, vadd3(nv,a.pos), a.rad }; }

// use the vector normal to the test surface to find the correct point along the shell of
//   the projectile.
typedef std::function<Projectile(World *, Projectile)> CollisionF;

void test_collide(World *w, Projectile pp, CollisionF cf) { Projectile res = pp;
  for(int i=0;i<w->t.size();i++) {
    if(in_triangle(proj_xz(pp.pos),w->t[i])&&pl_side_ball(w->p,pp,w->t[i])) {
      res = cf(w,res); } } w->p = res; }

Projectile rigid_elastic(World *w, Projectile pp) {
  // pp.vel = pp.vel - 2(pp.vel . normal)normal
  pp.vel = vsmul(DEGRADE,vsub3(pp.vel,vsmul(2*dot(pp.vel,w->t[0].norm),w->t[0].norm)));
  /*w->p.acc = pp.acc; w->p.vel = pp.vel;*/ pp.pos = w->p.pos; return pp; }
