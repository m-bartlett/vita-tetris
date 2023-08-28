#ifndef LINALG_H
#define LINALG_H value
#include <vitaGL.h>
#include <math.h>

void multiply(GLfloat *m, const GLfloat *n);
void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z);
void identity(GLfloat *m);
void transpose(GLfloat *m);
void invert(GLfloat *m);
void perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

// perspective_matrix(60,
//                     (float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT,
//                     1,
//                     1024);

#define perspective_matrix(fov_y, aspect, z_near, z_far) \
    { \
        [1]=0, [2]=0, [3]=0, [4]=0, \
        [6]=0, [7]=0, [8]=0, [9]=0, \
        [12]=0, [13]=0, [15]=0, \
        [0] = ( cos((fov_y/2)*(M_PI/180)) / sin((fov_y/2)*(M_PI/180)) ) / (aspect), \
        [5] = cos((fov_y/2)*(M_PI/180)) / sin((fov_y/2)*(M_PI/180)), \
        [10] = -(z_far+z_near)/(z_far-z_near), \
        [11] = -1, \
        [14] = -2 * (z_near*z_far) / (z_far-z_near) \
    }

#endif