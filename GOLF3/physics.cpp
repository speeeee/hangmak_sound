#include "world.hpp"
#include "bounds.hpp"

#include <memory>
// uses Projectile.

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile a) { Vec3 nv = vadd3(a.acc, a.vel);
  return (Projectile) { a.acc, nv, vadd3(nv,a.pos), a.rad }; }

Projectile test_collide(World *w, Projectile pp, Entity e) {
  Projectile orig = w->p; Projectile res = pp;
  orig.pos = vsub3(orig.pos,e.pos); res.pos = vsub3(res.pos,e.pos);
  if(e.bf(proj_xz(pp.pos))) {
    for(int i=0;i<e.t.size();i++) { 
      if(in_triangle(proj_xz(res.pos),e.t[i])&&pl_side_ball(orig,res,e.t[i])) {
        res = (e.cf)(w,orig,e.t[i],res); } } res.pos = vadd3(res.pos,e.pos); } return res; }
void entity_collide(World *w, Projectile pp) { Projectile res = pp;
  for(int j=0;j<w->e.size();j++) { res = test_collide(w,res,w->e[j]); } w->p = res; }

// world not used.
Projectile rigid_elastic(World *w, Projectile orig, Triangle t, Projectile pp) {
  // pp.vel = pp.vel - 2(pp.vel . normal)normal
  pp.vel = vsmul(DEGRADE,vsub3(pp.vel,vsmul(2*dot(pp.vel,t.norm),t.norm)));
  /*w->p.acc = pp.acc; w->p.vel = pp.vel;*/ pp.pos = orig.pos; return pp; }

Projectile no_react(World *w, Projectile orig, Triangle t, Projectile pp) { return pp; }
