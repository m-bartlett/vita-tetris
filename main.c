#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#define _GNU_SOURCE

#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <vitasdk.h>
#include <vitaGL.h>
#include "linalg.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

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
static GLfloat angle = 0.0;
static GLuint ModelViewProjectionMatrix_location,
              NormalMatrix_location,
              LightSourcePosition_location,
              MaterialColor_location;
static GLfloat ProjectionMatrix[16];
static const GLfloat LightSourcePosition[4] = { 5.0, 5.0, 10.0, 1.0};


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


static void draw_cube(GLfloat *transform,
                      GLfloat x,
                      GLfloat y,
                      GLfloat angle,
                      const GLfloat color[4]) {
   GLfloat model_view[16];
   GLfloat normal_matrix[16];
   GLfloat model_view_projection[16];

   /* Translate and rotate the cube */
                 
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

   /* Set the cube color */
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

   /* Draw the triangle strips that comprise the cube */
   // glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
   glDrawElements(/* mode */    GL_TRIANGLE_STRIP,
                  /* count */   ARRAY_SIZE(CUBE_FACE_STRIP_VERTEX_INDICES),
                  /* type */    GL_UNSIGNED_SHORT,
                  /* indices */ NULL); // NULL == use bound GL_ELEMENT_ARRAY_BUFFER

   /* Disable the attributes */
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

static void cube_draw(void) {
   const static GLfloat red[4] = { 1.0, 0.7, 0.4, 1.0 };
   GLfloat transform[16];
   identity(transform);

   glClearColor(0.5, 0.5, 0.5, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Translate and rotate the view */
   translate(transform, 0, 0, -20);
   rotate(transform, 2 * M_PI * view_rot[0] / 360.0, 1, 0, 0);
   rotate(transform, 2 * M_PI * view_rot[1] / 360.0, 0, 1, 0);
   rotate(transform, 2 * M_PI * view_rot[2] / 360.0, 0, 0, 1);

   /* Draw the cube */
   draw_cube(transform, 0, 0, angle, red);

   vglSwapBuffers(GL_FALSE);
}

static void cube_reshape(int width, int height) {
   perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);
   glViewport(0, 0, (GLint) width, (GLint) height);
}

static void cube_special(int special, int crap, int morecrap) {
   SceCtrlData pad;
   sceCtrlPeekBufferPositive(0, &pad, 1);
   if (pad.buttons & SCE_CTRL_LEFT)  view_rot[1] += 5.0;
   if (pad.buttons & SCE_CTRL_RIGHT) view_rot[1] -= 5.0;
   if (pad.buttons & SCE_CTRL_UP)    view_rot[0] += 5.0;
   if (pad.buttons & SCE_CTRL_DOWN)  view_rot[0] -= 5.0;
}

static void cube_idle(void) {
   static int frames = 0;
   static double tRot0 = -1.0, tRate0 = -1.0;
   double dt, t = (double)sceKernelGetProcessTimeWide() / 1000000.0;

   if (tRot0 < 0.0) tRot0 = t;
   dt = t - tRot0;
   tRot0 = t;

   /* advance rotation for next frame */
   angle += 70.0 * dt;  /* 70 degrees per second */
   if (angle > 3600.0) angle -= 3600.0;

   cube_special(0, 0, 0);
   cube_reshape(DISPLAY_WIDTH, DISPLAY_HEIGHT);
   cube_draw();
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


static void cube_init(void) {
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

   cube_init();
   while(1) { cube_idle(); }

   return 0;
}
