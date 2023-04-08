#include <vitaGL.h>

void multiply(GLfloat *m, const GLfloat *n);
void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z);
void identity(GLfloat *m);
void transpose(GLfloat *m);
void invert(GLfloat *m);
void perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);