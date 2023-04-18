#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#define _GNU_SOURCE

#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vitasdk.h>
#include <vitaGL.h>
#include "linalg.h"
#include "pgm.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20


static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
 {1,0,0,0,0,0,0,0,0,3},
 {2,3,0,0,0,0,0,0,0,0},
 {4,0,6,0,0,0,0,0,0,0},
 {7,1,2,3,0,0,0,0,0,0},
 {4,0,6,0,1,0,0,0,0,0},
 {2,3,0,5,6,7,0,0,0,0},
 {0,1,2,3,0,5,6,0,0,0},
 {0,0,7,1,2,3,4,5,0,0},
 {0,0,0,6,7,1,2,3,4,0},
 {0,0,0,0,5,6,7,1,2,3},
 {0,5,6,7,1,2,0,0,0,0},
 {1,3,0,5,6,7,1,0,0,0},
 {0,1,0,3,0,5,0,7,0,0},
 {0,0,0,2,0,4,0,6,0,3},
 {0,0,0,0,7,1,2,3,4,5},
 {0,0,0,0,0,6,7,1,2,3},
 {0,0,0,0,0,0,4,5,6,7},
 {0,0,0,0,0,0,0,1,2,3},
 {0,0,0,0,0,0,0,0,4,5},
 {0,0,0,0,0,0,0,0,0,6},
};


// static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,0,0,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,2,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
//     {2,0,2,2,2,2,2,2,2,2},
// };


#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define TYPE_LOCATION 2
// #define NORMAL_LOCATION 2

typedef struct {
    uint8_t x, y, z;
    uint8_t u, v;
    uint8_t block, face;
} vertex_t;

enum { FACE_FRONT = 0, FACE_TOP = 1, FACE_RIGHT = 2, FACE_BOTTOM = 3, FACE_LEFT = 4 };


#define PLAYFIELD_VERTEX_COUNT_MAX (6*5*PLAYFIELD_HEIGHT*PLAYFIELD_WIDTH)
static vertex_t VERTEX_BUFFER[PLAYFIELD_VERTEX_COUNT_MAX];
static uint32_t VERTEX_BUFFER_SIZE=0;

void parse_playfield_to_triangles() {
    GLboolean current_is_populated, previous_was_populated;
    uint8_t y=0, x=0, y1=0, x1=0, r=PLAYFIELD_HEIGHT-1;

    while (y < PLAYFIELD_HEIGHT) {
        y1 = y+1;
        x = 0;
        const char* row = PLAYFIELD[r--];

        while (x < PLAYFIELD_WIDTH) {
            const uint8_t B = row[x];
            x1 = x+1;
            if (B != 0) {

            	#define ADD_VERTEX(...) VERTEX_BUFFER[VERTEX_BUFFER_SIZE++]=(vertex_t){__VA_ARGS__}

                ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=0, .v=0, .block=B, .face=FACE_FRONT);
                ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=0, .v=1, .block=B, .face=FACE_FRONT);
                ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=1, .v=1, .block=B, .face=FACE_FRONT);

                ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=0, .v=0, .block=B, .face=FACE_FRONT);
                ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=1, .v=1, .block=B, .face=FACE_FRONT);
                ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=1, .v=0, .block=B, .face=FACE_FRONT);

                ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=0, .block=B, .face=FACE_LEFT);
                ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=0, .v=1, .block=B, .face=FACE_LEFT);
                ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=1, .block=B, .face=FACE_LEFT);

                ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=0, .block=B, .face=FACE_LEFT);
                ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=1, .block=B, .face=FACE_LEFT);
                ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=0, .block=B, .face=FACE_LEFT);

                ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=0, .v=0, .block=B, .face=FACE_RIGHT);
                ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=0, .v=1, .block=B, .face=FACE_RIGHT);
                ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=1, .v=1, .block=B, .face=FACE_RIGHT);

                ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=0, .v=0, .block=B, .face=FACE_RIGHT);
                ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=1, .v=1, .block=B, .face=FACE_RIGHT);
                ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=1, .v=0, .block=B, .face=FACE_RIGHT);

                ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=0, .v=0, .block=B, .face=FACE_TOP);
                ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=0, .v=1, .block=B, .face=FACE_TOP);
                ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=1, .v=1, .block=B, .face=FACE_TOP);

                ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=0, .v=0, .block=B, .face=FACE_TOP);
                ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=1, .v=1, .block=B, .face=FACE_TOP);
                ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=1, .v=0, .block=B, .face=FACE_TOP);

                ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=0, .block=B, .face=FACE_BOTTOM);
                ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=0, .v=1, .block=B, .face=FACE_BOTTOM);
                ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=1, .v=1, .block=B, .face=FACE_BOTTOM);

                ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=0, .block=B, .face=FACE_BOTTOM);
                ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=1, .v=1, .block=B, .face=FACE_BOTTOM);
                ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=1, .v=0, .block=B, .face=FACE_BOTTOM);

            }
            x = x1;
        }
        y = y1;
    }
}


void load_shader(const char *shader_path, GLuint shader_type, GLuint *program) {
   FILE *f = fopen(shader_path, "r");
   if (!f) {printf("Shader error\n\n"); return; }
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
                  /* array of string arrays */ (const char*[]){(const char*)&shader_body},
                  /* string length(s) */ &shader_size);
   glCompileShader(shader_ref);
   glAttachShader(*program, shader_ref);
}

static GLuint VertexBufferID_g, TextureID_g;
static GLfloat user_offset[3] = { 0.0, 0.0, -20.0 };
static GLuint ViewMatrix_location,
			  ModelMatrix_location,
              NormalMatrix_location,
              ProjectionMatrix_location,
              LightPosition_location;
static GLfloat ProjectionMatrix[16];
static const GLfloat LightPosition[3] = { 5.0, 5.0, -10.0};

static void cube_draw() {
   GLfloat model_matrix[16], view_matrix[16], normal_matrix[16], projection_matrix[16];

   identity(view_matrix);
   translate(view_matrix, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, user_offset[2]);
   glUniformMatrix4fv(ViewMatrix_location, 1, GL_FALSE, view_matrix);


   identity(model_matrix);
   translate(model_matrix, PLAYFIELD_WIDTH/2, PLAYFIELD_HEIGHT/2, 0);
   rotate(model_matrix, 2 * M_PI * user_offset[0] / 360.0, 1, 0, 0);
   rotate(model_matrix, 2 * M_PI * user_offset[1] / 360.0, 0, 1, 0);
   translate(model_matrix, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, 0);
   glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, model_matrix);


   memcpy(projection_matrix, ProjectionMatrix, sizeof(projection_matrix));
   // multiply(projection_matrix, view_matrix);
   glUniformMatrix4fv(ProjectionMatrix_location, 1, GL_FALSE, projection_matrix);


   memcpy(normal_matrix, view_matrix, sizeof (normal_matrix));
   invert(normal_matrix);
   transpose(normal_matrix);
   glUniformMatrix4fv(NormalMatrix_location, 1, GL_FALSE, normal_matrix);

   //////////////////////////////////////////////////////////////////////////////

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID_g);

   glEnableVertexAttribArray(POSITION_LOCATION);
   glVertexAttribPointer(/* location */  POSITION_LOCATION,
                         /* dimension */ 3,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   (GLvoid*)offsetof(vertex_t,x));
                              // If GL_ARRAY_BUFFER is bound, *pointer* is an offset into it

   glEnableVertexAttribArray(TEXCOORD_LOCATION);
   glVertexAttribPointer(/* location */  TEXCOORD_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   (GLvoid*)offsetof(vertex_t,u));

   glEnableVertexAttribArray(TYPE_LOCATION);
   glVertexAttribPointer(/* location */  TYPE_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   (GLvoid*)offsetof(vertex_t,block));

   glDrawArrays(/*mode=*/GL_TRIANGLES, /*first=*/0, /*count=*/VERTEX_BUFFER_SIZE);

   glDisableVertexAttribArray(POSITION_LOCATION);
   glDisableVertexAttribArray(TEXCOORD_LOCATION);
   glDisableVertexAttribArray(TYPE_LOCATION);

   vglSwapBuffers(GL_FALSE);
}

static void cube_reshape(int width, int height) {
   perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);
   glViewport(0, 0, (GLint) width, (GLint) height);
}

static inline void read_input() {
   SceCtrlData pad;
   sceCtrlPeekBufferPositive(0, &pad, 1);
   if (pad.buttons & SCE_CTRL_LEFT)  user_offset[1] += 5.0;
   if (pad.buttons & SCE_CTRL_RIGHT) user_offset[1] -= 5.0;
   if (pad.buttons & SCE_CTRL_UP)    user_offset[0] += 5.0;
   if (pad.buttons & SCE_CTRL_DOWN)  user_offset[0] -= 5.0;
   if (pad.buttons & SCE_CTRL_CROSS)  user_offset[2] -= 0.5;
   if (pad.buttons & SCE_CTRL_SQUARE) user_offset[2] += 0.5;
   // SCE_CTRL_CIRCLE
   // SCE_CTRL_TRIANGLE
}


static void cube_init(void) {
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CW); 
   glClearColor(0.1, 0.1, 0.1, 1.0);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   GLuint program = glCreateProgram();
   load_shader("app0:vertex.cg", GL_VERTEX_SHADER, &program);
   load_shader("app0:fragment.cg", GL_FRAGMENT_SHADER, &program);

   glBindAttribLocation(program, POSITION_LOCATION, "position");
   glBindAttribLocation(program, TEXCOORD_LOCATION, "texcoord");
   glBindAttribLocation(program, TYPE_LOCATION, "type");
   // glBindAttribLocation(program, NORMAL_LOCATION, "normal");

   glLinkProgram(program);
   glUseProgram(program);

   /* Get the locations of the uniforms so we can access them */
   ViewMatrix_location       = glGetUniformLocation(program, "ViewMatrix");
   ModelMatrix_location      = glGetUniformLocation(program, "ModelMatrix");
   ProjectionMatrix_location = glGetUniformLocation(program, "ProjectionMatrix");
   NormalMatrix_location     = glGetUniformLocation(program, "NormalMatrix");
   LightPosition_location    = glGetUniformLocation(program, "LightPosition");
   glUniform3fv(LightPosition_location, 1, LightPosition);

   parse_playfield_to_triangles();

   glGenBuffers(1, &VertexBufferID_g);
   glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID_g);
   glBufferData(/* type */  GL_ARRAY_BUFFER,
                /* size */  VERTEX_BUFFER_SIZE * sizeof(vertex_t),
                /* data */  VERTEX_BUFFER,
                /* usage */ GL_STATIC_DRAW);

   /* Load block image texture and bind to openGL */
   unsigned int texture_width, texture_height;
   uint8_t* texture_pixels;
   int pgm_error = pgm_load_image("app0:block.pgm", &texture_pixels,
                                                    &texture_width,
                                                    &texture_height);
   if (pgm_error) printf("pgm_read_image_metadata error: %d\n", pgm_error);
   else printf("Read block.pgm width=%d height=%d \n\n", texture_width, texture_height);

   glGenTextures(1, &TextureID_g);
   glBindTexture(GL_TEXTURE_2D, TextureID_g);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RED,
                /* width */  texture_width,
                /* height */ texture_height,
                /* border */ 0,
                /* format */ GL_RED,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   texture_pixels);
   free(texture_pixels);

   glUniform1i(glGetUniformLocation(program, "gTexture"), 0);
   glActiveTexture(GL_TEXTURE0);

}

int main(int argc, char *argv[]) {
   /* Initialize the window */
   vglInitExtended(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x800000, SCE_GXM_MULTISAMPLE_4X);

   cube_init();

   cube_reshape(DISPLAY_WIDTH, DISPLAY_HEIGHT);

   while(1) {
    read_input();
    cube_draw();
    // cube_idle();
   }

   return 0;
}
