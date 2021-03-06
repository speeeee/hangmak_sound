#include "loader.hpp"
#include "general_data.hpp"

GLuint create_program(const GLchar *vsh, const GLchar *fsh) { GLuint vs;
  vs = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vs,1,&vsh,NULL); glCompileShader(vs);
  
  GLint compiled = 0;
  glGetShaderiv(vs,GL_COMPILE_STATUS,&compiled);
  if(!compiled) { printf("Vertex error:\n"); GLint mlen = 0;
    glGetShaderiv(vs,GL_INFO_LOG_LENGTH, &mlen);
    std::vector<GLchar> error_log(mlen);
    glGetShaderInfoLog(vs,mlen,&mlen,&error_log[0]);
    glDeleteShader(vs); printf("%s\n",&error_log[0]); }
  GLuint fs; fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs,1,&fsh,NULL); glCompileShader(fs);
  compiled = 0;
  glGetShaderiv(fs,GL_COMPILE_STATUS,&compiled);
  if(!compiled) { printf("Fragment error:\n"); GLint mlen = 0;
    glGetShaderiv(fs,GL_INFO_LOG_LENGTH, &mlen);
    std::vector<GLchar> error_log(mlen);
    glGetShaderInfoLog(fs,mlen,&mlen,&error_log[0]);
    glDeleteShader(fs); printf("%s\n",&error_log[0]); }

  GLuint prog; prog = glCreateProgram();
  glAttachShader(prog,vs); glAttachShader(prog,fs);
  /*glBindAttribLocation(prog,0,"position");
  glBindAttribLocation(prog,1,"norm");*/ glLinkProgram(prog);
  glDeleteShader(vs); glDeleteShader(fs); return prog; }

void load(World *w, int hole) { switch(hole) {
  // TODO: lift triangulate to somewhere else.
  case 0: {
    w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0.65,1,4.),0.05);
    const float t0 = M_PI/10.; const float sz = 0.03;
    w->e = create_entities({ einit(rigid_elastic,triangulate(hole_0,EX_STEP,EX_NSTEPS),v3(0,0,0)
                                  ,EX_STEP,EX_NSTEPS,EX_NSTEPS*2,true),
                             einit(no_react,grass_blade(sz,0.1,2,0,M_PI/2),v3(0,0,0)
                                  ,0,1,5/* 2*detail+1 */,false),
                             einit(no_react,grass_blade(sz,0.1,2,t0,M_PI/4.),v3(0,0,0)
                                  ,0,1,5,false),
                             einit(no_react,grass_blade(sz,0.1,2,t0,3*M_PI/4.),v3(0,0,0)
                                  ,0,1,5,false),
                             einit(no_react,grass_blade(sz,0.1,2,t0,5*M_PI/4.),v3(0,0,0)
                                  ,0,1,5,false),
                             einit(no_react,grass_blade(sz,0.1,2,t0,7*M_PI/4.),v3(0,0,0)
                                  ,0,1,5,false),
                             einit(no_react,ball(0.05,20),v3(0,0,0),0,1,21,false) });
  // DONE: put all of this in new construction function.
    w->e[0].shader_id = create_program(sample_vs,sample_fs);
    w->e[1].shader_id = create_program(grass_vs,grass_fs);
    for(int i=2;i<6;i++) { w->e[i].shader_id = w->e[1].shader_id; } break; }
  case 1: { w->p = projectile(v3(0,GRAVITY,0),v3(0,0,0),v3(0.65,1.,4.),0.05);
    std::vector<float> tr = cyl_to_tris(test_cyl_0,M_PI/100.,0.01,0,201,125);
    std::vector<float> cap = stride_copy(tr.begin(),tr.begin()+201*STRIDE*2,STRIDE*2,STRIDE);
    for(int i=0;i<cap.size();i+=6) { cap[i+3] = 0; cap[i+4] = -1; cap[i+5] = 0; }
    asadd(v3(0,0.05*25,0),&cap[0],cap.size(),STRIDE);
    w->e = create_entities({ einit(no_react,tr,v3(0,0,0)
                                  ,0,125,201*2,false),
                             einit(no_react,cyl_to_tris(test_cyl_0,M_PI/50.,0.02,0,101,63),v3(0,0,0)
                                  ,0,63,101*2,false),
                             //einit(no_react,tree,0,50,
                             einit(no_react,cap,v3(0,0,0),0,1,201,false),
                             einit(no_react,ball(0.05,20),v3(0,0,0),0,1,21,false) });
    // create circles with diameters on the sides of a regular N-side polygon.
    w->e[0].shader_id = create_program(tree_vs,tree_fs);
    w->e[1].shader_id = w->e[0].shader_id;
    glUseProgram(w->e[0].shader_id);
    GLint _col = glGetUniformLocation(w->e[0].shader_id,"col");
    glUniform3f(_col,0.5,0.5,0.); break; } } }

void unload(World *w, int hole) { switch(hole) {
  case 0: { glDeleteVertexArrays(1,&w->e[0].vd.vao); } } }

// requires that given shader program has vec3 named disp.
void set_disp(GLuint prog, Vec3 v) {
  GLint _disp = glGetUniformLocation(prog,"disp");
  glUniform3f(_disp,v.x,v.y,v.z); }
