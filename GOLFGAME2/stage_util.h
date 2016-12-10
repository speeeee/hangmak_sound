#include <stdlib.h>

#include <glad/glad.h>
#include "glob_structs.h"

Plane plane(Array, int, Vec3);
// renders 2D surface at normal p.normal.
void render_plane(Plane);
void render_surface(Surface, GLfloat);
