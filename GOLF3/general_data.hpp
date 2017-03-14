#ifndef GENERAL_DATA_HPP
#define GENERAL_DATA_HPP

#include <cmath>
#include "world.hpp"

std::vector<float> ball(float rad, int detail) { std::vector<float> ret(detail*6+6);
  ret[0] = 0; ret[1] = 0; ret[2] = 0;
  ret[3] = 0; ret[4] = 0; ret[5] = -1; int i;
  for(float f=0,i=0; f<2*M_PI&&i<ret.size(); f+=2*M_PI/(float)detail,i+=6) {
    ret[i] = rad*cos(f); ret[i+1] = rad*sin(f); ret[i+2] = 0;
    ret[i+3] = 0; ret[i+4] = 0; ret[i+5] = 0; } return ret; }
#endif
