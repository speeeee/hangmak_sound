#include "world.hpp"

// this file renders several different types of blades of grass for use with
//   instancing in the main file.
// TODO: create different types of grass
//     : each blade is comprised of a sequence of quads.
//     : the blades will be stored at the back of the entities array,
//         the blades being their own entities.  Each vertex has its own normal, though a special
//         case must be considered for the back face.
//     : all animation is handled in the vertex shader.

// in vertex shader:
/* grass will render in tiles.  for each tile, 16x16 (number not official) blades
 * will be rendered, each in a smaller gridspace within the tile.  To seem more random,
 * there will be a small random displacement for each blade.  As the camera zooms out, the amount of
 * blades rendered will be halved in tiles far enough away.  All types of blades are
 * treated the same in the vertex shader.  The grass blades are animated by a sine function.
 * The period of this functions increases with the wind intensity.
 * The wind direction is parallel to the "direction" of the sine function.
 *
 * Finally, each blade is rotated by a random amount along the y-axis.
 *
 * The vertex shader requires the wind vector and 
 * the function of the surface the grass is rendered on. */

// renders grass blade of width and height.  The detail is the amount of quads that comprise the
//   blade.  Note that with less detail, the grass will react more rigidly to the wind.
//   arc is a signed value that represents the intensity of the arc of the blade.  How the arc
//   of the blade exactly relates to the value is to be determined TODO.
std::vector<float> grass_blade(float width, float height, int detail, float arc);
