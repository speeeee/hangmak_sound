#include "grass.hpp" 

// normally on XY plane.
// t0 = rotation along (1,0,0), t1 = rotation along (0,1,0)
// the rendering of the grass blade tends toward black at the last detail level.
//   this is because the normal of the final vertex are (0,0,0).
//   this can be fixed with a special case for the final level, but for now it works.
std::vector<float> grass_blade(float width, float height, int detail, float t0, float t1) {
  float slope = height/(width/2.); // x = (w/2)-y*(1/slope).
  std::vector<float> ret((detail+1)*3*2*2); // floats_per_pt*pts_per_group*n_attributes
  // allow for rotations here.
  // ||||||
  for(int i=0;i<(detail+1)*3*2*2;i+=3*2*2) { int d = i/(3*2*2);
    float y = height/detail*d;
    Vec3 ai = v3(-width/2.+y/slope,y,0); Vec3 bi = v3(width/2-y/slope,y,0);
    Vec3 norm;
    if(d%2) { Vec3 ci = v3(width/2.-height/detail*(d+1)/slope,height/detail*(d+1),0);
              norm = norm_positive(bi,ai,ci);
              ret[i] = ai.x; ret[i+1] = ai.y; ret[i+2] = ai.z;
              ret[i+6] = bi.x; ret[i+7] = bi.y; ret[i+8] = bi.z; }
    else { Vec3 ci = v3(-width/2.+height/detail*(d+1)/slope,height/detail*(d+1),0);;
           norm = norm_positive(ai,bi,ci);
           ret[i] = bi.x; ret[i+1] = bi.y; ret[i+2] = bi.z;
           ret[i+6] = ai.x; ret[i+7] = ai.y; ret[i+8] = ai.z; }
    ret[i+3] = ret[i+9] = norm.x; ret[i+4] = ret[i+10] = norm.y;
    ret[i+5] = ret[i+11] = norm.z; } return ret; }
