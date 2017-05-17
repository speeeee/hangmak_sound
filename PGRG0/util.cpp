#include "util.hpp"
#include <tuple>

std::vector<Entity> create_entities(std::vector<std::vector<float>> v) {
  std::vector<Entity> ret; std::vector<float> dat;
  GLuint vao; glGenVertexArrays(1,&vao);
  for(int i=0;i<v.size();i++) { int pos = dat.size()/STRIDE; // load array into total array.
    dat.reserve(dat.size()+v[i].size());
    dat.insert(dat.end(),v[i].begin(),v[i].end());
    // create entity from array.  v[i]'s size is guaranteed to be a multiple of STRIDE.
    //   dat's size is guaranteed to be a multiple of v[i]'s size and by extension STRIDE.
    ret.push_back(Entity(VAOdat(pos,v[i].size()/STRIDE,vao),0)); }
  //for(int i=0;i<dat.size();i++) { printf("%g ",dat[i]); }
  glBindVertexArray(vao);
  GLuint buf; glGenBuffers(1,&buf);
  glBindBuffer(GL_ARRAY_BUFFER, buf);
  glBufferData(GL_ARRAY_BUFFER,dat.size()*sizeof(float),&dat[0],GL_STATIC_DRAW);

  GLuint pos_attrib = 0; GLuint col_attrib = 1; GLuint norm_attrib = 2;
  glEnableVertexAttribArray(pos_attrib); // attribute location, e.g. (location = 0) for position.
  glVertexAttribPointer(pos_attrib,3,GL_FLOAT,GL_FALSE,STRIDE*sizeof(GL_FLOAT),0);

  glEnableVertexAttribArray(col_attrib); // attribute location, e.g. (location = 1) for color.
  glVertexAttribPointer(col_attrib,3,GL_FLOAT,GL_FALSE,STRIDE*sizeof(GL_FLOAT)
                       ,(const GLvoid *)(3*sizeof(GL_FLOAT)));

  glEnableVertexAttribArray(norm_attrib); // attribute location, e.g. (location = 2) for normal.
  glVertexAttribPointer(norm_attrib,3,GL_FLOAT,GL_FALSE,STRIDE*sizeof(GL_FLOAT)
                       ,(const GLvoid *)(6*sizeof(GL_FLOAT)));
  return ret; }
