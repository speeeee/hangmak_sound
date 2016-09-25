#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>

typedef struct { GLfloat cxz; GLfloat cyz; } Camera;
typedef struct { GLfloat x; GLfloat y; GLfloat z; GLfloat tht; GLfloat phi; } KState;

typedef struct Queue { struct Queue *n; } Queue;
typedef struct { int t; int act; } Instr;

typedef struct { GLfloat x; GLfloat y; GLfloat z; } Player;
typedef struct { Player pl; Camera ca; KState lk; int t;
                 Instr *evs; int esz; } GState;
