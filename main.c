#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#define _GNU_SOURCE

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <vitasdk.h>
#include <vitaGL.h>
// #include "shader.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

#ifndef HAVE_BUILTIN_SINCOS
#define sincos _sincos
static void
sincos (double a, double *s, double *c)
{
  *s = sin (a);
  *c = cos (a);
}
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

const int16_t CUBE_VERTICES[] = {
 /*0*/  1, 1, 1,   /*1*/  1, 1,-1,   /*2*/  1,-1, 1,   /*3*/  1,-1,-1, 
 /*4*/ -1, 1, 1,   /*5*/ -1, 1,-1,   /*6*/ -1,-1, 1,   /*7*/ -1,-1,-1
};

const int16_t CUBE_VERTEX_NORMALS[] = {
 /*0*/  1, 1, 1,   /*1*/  1, 1,-1,   /*2*/  1,-1, 1,   /*3*/  1,-1,-1, 
  /*4*/ -1, 1, 1,   /*5*/ -1, 1,-1,   /*6*/ -1,-1, 1,   /*7*/ -1,-1,-1
};

const uint16_t CUBE_FACE_STRIP_VERTEX_INDICES[] = { // Triangle-strip indices that wind clockwise
   6,2,4,0,1,2,3,6,7,4,5,1 
   // 6,2,4,0,0,1,1,3,5,7
};
const size_t CUBE_FACE_STRIP_VERTEX_INDICES_SIZE = ARRAY_SIZE(CUBE_FACE_STRIP_VERTEX_INDICES);


#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20
const char PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
   "J000II00ZS",
   "00I0Z0L000",
   "T00O0IZT0L",
   "000LZL00JO",
   "000000I000",
   "TLL0000J00",
   "000OILIZJI",
   "J0TLLTSTOZ",
   "LL000000JZ",
   "JO0J000000",
   "L0LIIT0I00",
   "0J000000O0",
   "IL00O00ZLI",
   "T00I0LL0OL",
   "0LZTOTSJ0J",
   "0SI0SJ0000",
   "STZO00IT0T",
   "00O00LT0TI",
   "O0SZ0LS00J",
   "JO00TTOL0L",
};


static GLint VBOglobal, IBOglobal;


static GLfloat view_rot[3] = { 20.0, 30.0, 0.0 };
static struct cube *cube1, *cube2, *cube3;
static GLfloat angle = 0.0;
static GLuint ModelViewProjectionMatrix_location,
              NormalMatrix_location,
              LightSourcePosition_location,
              MaterialColor_location;
static GLfloat ProjectionMatrix[16];
static const GLfloat LightSourcePosition[4] = { 5.0, 5.0, 10.0, 1.0};


static void multiply(GLfloat *m, const GLfloat *n) { //{{{
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

static void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { //{{{
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

static void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z) {
   GLfloat t[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };
   multiply(m, t);
}

static void identity(GLfloat *m) { //{{{
   GLfloat t[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0,
   };

   memcpy(m, t, sizeof(t));
/*}}}*/ }

static void transpose(GLfloat *m) { //{{{
   GLfloat t[16] = {
      m[0], m[4], m[8],  m[12],
      m[1], m[5], m[9],  m[13],
      m[2], m[6], m[10], m[14],
      m[3], m[7], m[11], m[15]};
   memcpy(m, t, sizeof(t));
/*}}}*/ }

static void invert(GLfloat *m) { //{{{
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


void load_shader(const char *shader_path, GLuint shader_type, GLuint *program) {
   FILE *f = fopen(shader_path, "r");
   if (!f) {
     printf("Shader error\n\n");
     exit(1);
     return;
   }
   fseek(f, 0, SEEK_END);
   uint32_t shader_size = ftell(f);
   fseek(f, 0, SEEK_SET);
   char shader_body[shader_size];
   fread(shader_body, 1, shader_size, f);
   fclose(f);
   printf("%s\n\n", shader_body);

   GLuint shader_ref = glCreateShader(shader_type);
   glShaderSource(/* shader */ shader_ref,
                  /* sources count */ 1,
                  /* array of string arrays */ (const char*[]){&shader_body},
                  /* string length(s) */ &shader_size);
   glCompileShader(shader_ref);
   glAttachShader(*program, shader_ref);
}


static void draw_gear(GLfloat *transform,
                      GLfloat x,
                      GLfloat y,
                      GLfloat angle,
                      const GLfloat color[4]) {
   GLfloat model_view[16];
   GLfloat normal_matrix[16];
   GLfloat model_view_projection[16];

   /* Translate and rotate the gear */
                 
   memcpy(model_view, transform, sizeof (model_view));
   translate(model_view, x, y, 0);
   rotate(model_view, 2 * M_PI * angle / 360.0, 0, 0, 1);

   /* Create and set the ModelViewProjectionMatrix */
   memcpy(model_view_projection, ProjectionMatrix, sizeof(model_view_projection));
   multiply(model_view_projection, model_view);

   glUniformMatrix4fv(ModelViewProjectionMatrix_location, 1, GL_FALSE,
                      model_view_projection);

   /* Create and set the NormalMatrix. It's the inverse transpose of the ModelView matrix. */
   memcpy(normal_matrix, model_view, sizeof (normal_matrix));
   invert(normal_matrix);
   transpose(normal_matrix);
   glUniformMatrix4fv(NormalMatrix_location, 1, GL_FALSE, normal_matrix);

   /* Set the gear color */
   glUniform4fv(MaterialColor_location, 1, color);

   /* Set the vertex buffer object to use */
   glBindBuffer(GL_ARRAY_BUFFER, VBOglobal);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOglobal);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(/* location */   0,
                         /* dimensions */ 3,
                         /* type */       GL_SHORT,
                         /* normalized */ GL_FALSE,
                         /* stride */     3 * sizeof(int16_t),
                         /* pointer */    NULL); // NULL == use bound GL_ARRAY_BUFFER

   glBindBuffer(GL_ARRAY_BUFFER, 0); // Not sure why this is needed, but it is.
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(/* location */   1,
                         /* dimensions */ 3,
                         /* type */       GL_SHORT,
                         /* normalized */ GL_FALSE,
                         /* stride */     3 * sizeof(int16_t),
                         /* pointer */    CUBE_VERTEX_NORMALS);

   /* Draw the triangle strips that comprise the gear */
   // glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
   glDrawElements(/* mode */    GL_TRIANGLE_STRIP,
                  /* count */   ARRAY_SIZE(CUBE_FACE_STRIP_VERTEX_INDICES),
                  /* type */    GL_UNSIGNED_SHORT,
                  /* indices */ NULL); // NULL == use bound GL_ELEMENT_ARRAY_BUFFER

   /* Disable the attributes */
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

static void gears_draw(void) {
   const static GLfloat red[4] = { 0.8, 0.1, 0.4, 1.0 };
   GLfloat transform[16];
   identity(transform);

   glClearColor(0.5, 0.5, 0.5, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Translate and rotate the view */
   translate(transform, 0, 0, -20);
   rotate(transform, 2 * M_PI * view_rot[0] / 360.0, 1, 0, 0);
   rotate(transform, 2 * M_PI * view_rot[1] / 360.0, 0, 1, 0);
   rotate(transform, 2 * M_PI * view_rot[2] / 360.0, 0, 0, 1);

   /* Draw the gears */
   draw_gear(transform, 0, 0, angle, red);

   vglSwapBuffers(GL_FALSE);
}

static void gears_reshape(int width, int height) {
   perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);
   glViewport(0, 0, (GLint) width, (GLint) height);
}

static void gears_special(int special, int crap, int morecrap) {
   SceCtrlData pad;
   sceCtrlPeekBufferPositive(0, &pad, 1);
   if (pad.buttons & SCE_CTRL_LEFT)
         view_rot[1] += 5.0;
   if (pad.buttons & SCE_CTRL_RIGHT)
         view_rot[1] -= 5.0;
   if (pad.buttons & SCE_CTRL_UP)
         view_rot[0] += 5.0;
   if (pad.buttons & SCE_CTRL_DOWN)
         view_rot[0] -= 5.0;
}

static void gears_idle(void) {
   static int frames = 0;
   static double tRot0 = -1.0, tRate0 = -1.0;
   double dt, t = (double)sceKernelGetProcessTimeWide() / 1000000.0;

   if (tRot0 < 0.0) tRot0 = t;
   dt = t - tRot0;
   tRot0 = t;

   /* advance rotation for next frame */
   angle += 70.0 * dt;  /* 70 degrees per second */
   if (angle > 3600.0) angle -= 3600.0;

   gears_special(0, 0, 0);
   gears_reshape(DISPLAY_WIDTH, DISPLAY_HEIGHT);
   gears_draw();
   frames++;

   if (tRate0 < 0.0)
      tRate0 = t;
   if (t - tRate0 >= 5.0) {
      GLfloat seconds = t - tRate0;
      GLfloat fps = frames / seconds;
      printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
            fps);
      tRate0 = t;
      frames = 0;
   }
}


static void gears_init(void) {
   GLuint v, f, program;
   const char *p;
   char msg[512];

   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   program = glCreateProgram();
   load_shader("app0:vertex.cg", GL_VERTEX_SHADER, &program);
   load_shader("app0:fragment.cg", GL_FRAGMENT_SHADER, &program);


   glBindAttribLocation(program, 0, "position");
   glBindAttribLocation(program, 1, "normal");

   glLinkProgram(program);

   /* Enable the shaders */
   glUseProgram(program);
   /* Get the locations of the uniforms so we can access them */
   ModelViewProjectionMatrix_location = glGetUniformLocation(program, "ModelViewProjectionMatrix");
   NormalMatrix_location = glGetUniformLocation(program, "NormalMatrix");
   LightSourcePosition_location = glGetUniformLocation(program, "LightSourcePosition");
   MaterialColor_location = glGetUniformLocation(program, "MaterialColor");
   /* Set the LightSourcePosition uniform which is constant throught the program */
   glUniform4fv(LightSourcePosition_location, 1, LightSourcePosition);
   
   glGenBuffers(1, &VBOglobal);
   glBindBuffer(GL_ARRAY_BUFFER, VBOglobal);
   glBufferData(/* target */ GL_ARRAY_BUFFER,
                /* size */   8 * 3 * sizeof(int16_t),
                /* data */   CUBE_VERTICES,
                /* usage */  GL_STATIC_DRAW);

   GLuint index_buffer;
   glGenBuffers(1, &IBOglobal);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOglobal);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                sizeof(CUBE_FACE_STRIP_VERTEX_INDICES),
                CUBE_FACE_STRIP_VERTEX_INDICES,
                GL_STATIC_DRAW);

}

int
main(int argc, char *argv[])
{
   /* Initialize the window */
   vglInitExtended(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x800000, SCE_GXM_MULTISAMPLE_4X);

   gears_init();
   for (;;) { gears_idle(); }

   return 0;
}
