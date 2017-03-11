#include "world.hpp"

#include <memory>
// uses Projectile.

// constant for gravity.  expected to be set to y-val of acceleration multiplied by a number
//   representing mass.

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile);

// if pl_side is true, add normal vector scaled to current.
// NOTE: tests collision wrt Entity position.  Triangle positions should be relative to the
//     : entity position, and are treated as such in test_collide.
void test_collide(World *, Projectile, CollisionF, BoundsCompare);
void entity_collide(World *, Projectile, BoundsCompare);
Projectile rigid_elastic(World *, Projectile, Triangle, Projectile);
Projectile no_react(World *, Projectile, Triangle, Projectile);
