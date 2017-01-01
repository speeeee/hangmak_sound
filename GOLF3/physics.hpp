#include "world.hpp"
// uses Projectile.

// constant for gravity.  expected to be set to y-val of acceleration multiplied by a number
//   representing mass.
#define GRAVITY (-0.0005)

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile);

// if pl_side is true, add normal vector scaled to current.
