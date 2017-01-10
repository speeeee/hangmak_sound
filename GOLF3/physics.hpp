#include "world.hpp"

#include <memory>
// uses Projectile.

// constant for gravity.  expected to be set to y-val of acceleration multiplied by a number
//   representing mass.

// next state of a projectile's motion ignoring collisions.
Projectile next_state(Projectile);

// if pl_side is true, add normal vector scaled to current.
void test_collide(World *, Projectile, CollisionF);
void entity_collide(World *, Projectile);
Projectile rigid_elastic(World *, Triangle, Projectile);
