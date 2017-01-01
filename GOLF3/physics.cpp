#include "world.hpp"
// uses Projectile.

// constant for gravity.  expected to be set to y-val of acceleration multiplied by a number
//   representing mass.
#define GRAVITY (-0.01)

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile a) { Vec3 nv = vadd3(a.acc, a.vel);
  return (Projectile) { a.acc, nv, vadd3(nv,a.pos) }; }

// use the vector normal to the test surface to find the correct point along the shell of
//   the projectile.
