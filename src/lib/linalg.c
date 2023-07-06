#include "linalg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef HAVE_BUILTIN_SINCOS
#define sincos _sincos
static void
sincos (double a, double *s, double *c)
{
  *s = sin (a);
  *c = cos (a);
}
#endif


void multiply(GLfloat *m, const GLfloat *n) { //{{{
   GLfloat tmp[16];
   const GLfloat *row, *column;
   div_t d;
   int i, j;

   for (i = 0; i < 16; i++) {
      tmp[i] = 0;
      d = div(i, 4);
      row = n + d.quot * 4;
      column = m + d.rem;
      for (j = 0; j < 4; j++)
         tmp[i] += row[j] * column[j * 4];
   }
   memcpy(m, &tmp, sizeof tmp);
/*}}}*/ }

void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { //{{{
   double s, c;

   sincos(angle, &s, &c);
   GLfloat r[16] = {
      x * x * (1 - c) + c,     y * x * (1 - c) + z * s, x * z * (1 - c) - y * s, 0,
      x * y * (1 - c) - z * s, y * y * (1 - c) + c,     y * z * (1 - c) + x * s, 0, 
      x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, z * z * (1 - c) + c,     0,
      0, 0, 0, 1
   };

   multiply(m, r);
/*}}}*/ }

void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z) { //{{{
   GLfloat t[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };
   multiply(m, t);
/*}}}*/ }

void identity(GLfloat *m) { //{{{
   GLfloat t[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0,
   };

   memcpy(m, t, sizeof(t));
/*}}}*/ }

void transpose(GLfloat *m) { //{{{
   GLfloat t[16] = {
      m[0], m[4], m[8],  m[12],
      m[1], m[5], m[9],  m[13],
      m[2], m[6], m[10], m[14],
      m[3], m[7], m[11], m[15]};
   memcpy(m, t, sizeof(t));
/*}}}*/ }

void invert(GLfloat *m) { //{{{
   GLfloat t[16];
   identity(t);
   t[12] = -m[12]; t[13] = -m[13]; t[14] = -m[14];
   m[12] = m[13] = m[14] = 0;
   transpose(m);
   multiply(m, t);
/*}}}*/ }

void perspective(GLfloat *m,
                 GLfloat fovy,
                 GLfloat aspect,
                 GLfloat zNear,
                 GLfloat zFar) { //{{{
   GLfloat tmp[16];
   identity(tmp);

   double sine, cosine, cotangent, deltaZ;
   GLfloat radians = fovy / 2 * M_PI / 180;

   deltaZ = zFar - zNear;
   sincos(radians, &sine, &cosine);

   if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
      return;

   cotangent = cosine / sine;

   tmp[0] = cotangent / aspect;
   tmp[5] = cotangent;
   tmp[10] = -(zFar + zNear) / deltaZ;
   tmp[11] = -1;
   tmp[14] = -2 * zNear * zFar / deltaZ;
   tmp[15] = 0;

   memcpy(m, tmp, sizeof(tmp));
/*}}}*/ }