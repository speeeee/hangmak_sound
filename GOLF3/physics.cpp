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

Projectile test_collide(World *w, Projectile pp, Entity e) { Projectile res = pp;
  for(int i=0;i<e.t.size();i++) {
    if(in_triangle(proj_xz(res.pos),e.t[i])&&pl_side_ball(w->p,res,e.t[i])) {
      res = (e.cf)(w,e.t[i],res); } } return res; }
void entity_collide(World *w, Projectile pp) { Projectile res = pp;
  for(int j=0;j<w->e.size();j++) { res = test_collide(w,res,w->e[j]); } w->p = res; }
    

Projectile rigid_elastic(World *w, Triangle t, Projectile pp) {
  // pp.vel = pp.vel - 2(pp.vel . normal)normal
  pp.vel = vsmul(DEGRADE,vsub3(pp.vel,vsmul(2*dot(pp.vel,t.norm),t.norm)));
  /*w->p.acc = pp.acc; w->p.vel = pp.vel;*/ pp.pos = w->p.pos; return pp; }
