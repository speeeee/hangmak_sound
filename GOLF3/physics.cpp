#include "world.hpp"
#include "bounds.hpp"

#include <memory>
// uses Projectile.

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile a) { Vec3 nv = vadd3(a.acc, a.vel);
  return (Projectile) { a.acc, nv, vadd3(nv,a.pos), a.rad }; }

// use the vector normal to the test surface to find the correct point along the shell of
//   the projectile.
typedef std::function<Projectile(World *, Triangle, Projectile)> CollisionF;

void test_collide(World *w, Projectile pp, CollisionF cf) { Projectile res = pp;
  for(int i=0;i<w->t.size();i++) {
    if(in_triangle(proj_xz(res.pos),w->t[i])&&pl_side_ball(w->p,res,w->t[i])) {
      res = cf(w,w->t[i],res); } } w->p = res; }

Projectile rigid_elastic(World *w, Triangle t, Projectile pp) {
  // pp.vel = pp.vel - 2(pp.vel . normal)normal
  pp.vel = vsmul(DEGRADE,vsub3(pp.vel,vsmul(2*dot(pp.vel,t.norm),t.norm)));
  /*w->p.acc = pp.acc; w->p.vel = pp.vel;*/ pp.pos = w->p.pos; return pp; }
