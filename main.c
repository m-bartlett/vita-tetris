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
// #include "pgm.h"
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #include "lodepng.h"


#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20


static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
 {1,0,0,0,0,0,0,0,0,3},
 {2,3,0,0,0,0,0,0,0,0},
 {4,0,1,0,0,0,0,0,0,0},
 {7,1,1,3,0,0,0,0,0,0}, 
 {4,0,6,0,1,0,0,0,0,0},
 {2,3,0,5,6,7,0,0,0,0},
 {0,1,2,3,0,5,6,0,0,0},
 {0,0,7,1,2,3,4,5,0,0},
 {0,0,0,6,7,1,2,3,4,0},
 {0,0,0,0,5,7,6,1,2,3},
 {0,5,6,7,7,7,0,0,0,0},
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
} tetromino_vertex_t;

enum {FACE_FRONT = 0, FACE_TOP = 1, FACE_RIGHT = 2, FACE_BOTTOM = 3, FACE_LEFT = 4, FACE_QUANTITY};


#define PLAYFIELD_VERTEX_COUNT_MAX (6*5*PLAYFIELD_HEIGHT*PLAYFIELD_WIDTH)
static tetromino_vertex_t PLAYFIELD_VERTEX_BUFFER[PLAYFIELD_VERTEX_COUNT_MAX];
static uint32_t PLAYFIELD_VERTEX_BUFFER_SIZE=0;
#define ADD_VERTEX(...) \
   PLAYFIELD_VERTEX_BUFFER[PLAYFIELD_VERTEX_BUFFER_SIZE++]=(tetromino_vertex_t){__VA_ARGS__}


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


void load_shader(const char *shader_path, GLuint *program) {
   GLuint shader_type;
   if (strstr(shader_path, ".vert") == NULL) shader_type = GL_FRAGMENT_SHADER;
   else {
      shader_type = GL_VERTEX_SHADER;
      printf("VERTEX SHADER: %s\n", shader_path);
   }
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


static inline void flip_rgb_image_vertically(uint8_t* buffer,
                                         unsigned int width,
                                         unsigned int height) {
    /* Flip the image vertically, OpenGL expects (0,0) to be the bottom-left of the image */
   const unsigned int row_size = width * 3;
   unsigned int top_row_index = 0;
   unsigned int bottom_row_index = (height - 1) * row_size;
   uint8_t temp_row[row_size];
   for (int y = 0; y < height / 2; y++) {
      memcpy(temp_row, &buffer[top_row_index], row_size);
      memcpy(&buffer[top_row_index], &buffer[bottom_row_index], row_size);
      memcpy(&buffer[bottom_row_index], temp_row, row_size);
      top_row_index += row_size;
      bottom_row_index -= row_size;
   }
}


static GLuint ViewMatrix_location,
              ModelMatrix_location,
              ProjectionMatrix_location,
              LightPosition_location;
#define Z_INITIAL_OFFSET -20
static GLfloat user_offset[3] = { 0.0, 0.0, Z_INITIAL_OFFSET };
static GLfloat ViewMatrix[16] = { [0] = 1, [5] = 1, [10] = 1, [15] = 1,
                                  [12]=-PLAYFIELD_WIDTH/2,
                                  [13]=-PLAYFIELD_HEIGHT/2,
                                  [14]=Z_INITIAL_OFFSET, };
static GLfloat ModelMatrix[16] = { [0]=1, [5]=1, [10]=1, [15]=1 };
static GLfloat LightPosition[3] = { 0.0, 0.0, 100.0};


static inline void read_input() {
   SceCtrlData pad;
   sceCtrlPeekBufferPositive(0, &pad, 1);
   
   GLboolean view_matrix_needs_update = GL_FALSE;
   if (pad.buttons & SCE_CTRL_CROSS)  { user_offset[2] -= 0.5; view_matrix_needs_update=GL_TRUE; }
   if (pad.buttons & SCE_CTRL_SQUARE) { user_offset[2] += 0.5; view_matrix_needs_update=GL_TRUE; }
   // SCE_CTRL_CIRCLE
   // SCE_CTRL_TRIANGLE
   if (view_matrix_needs_update) {
      identity(ViewMatrix);
      translate(ViewMatrix, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, user_offset[2]);
      glUniformMatrix4fv(ViewMatrix_location, 1, GL_FALSE, ViewMatrix);
   }

   GLboolean model_matrix_needs_update = GL_FALSE;
   if (pad.buttons & SCE_CTRL_LEFT)  { user_offset[1] -= 5.0; model_matrix_needs_update=GL_TRUE; }
   if (pad.buttons & SCE_CTRL_RIGHT) { user_offset[1] += 5.0; model_matrix_needs_update=GL_TRUE; }
   if (pad.buttons & SCE_CTRL_UP)    { user_offset[0] -= 5.0; model_matrix_needs_update=GL_TRUE; }
   if (pad.buttons & SCE_CTRL_DOWN)  { user_offset[0] += 5.0; model_matrix_needs_update=GL_TRUE; }

   if (model_matrix_needs_update) {
      identity(ModelMatrix);
      translate(ModelMatrix, PLAYFIELD_WIDTH/2, PLAYFIELD_HEIGHT/2, 0);
      rotate(ModelMatrix, 2 * M_PI * user_offset[0] / 360.0, 1, 0, 0);
      rotate(ModelMatrix, 2 * M_PI * user_offset[1] / 360.0, 0, 1, 0);
      translate(ModelMatrix, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, 0);
      glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, ModelMatrix);
   }

   #define ANALOGS_DEADZONE 1
   GLboolean lighting_needs_update = GL_FALSE;
   int rx = pad.rx - 127, ry = pad.ry - 127;

   if (abs(rx) > ANALOGS_DEADZONE) {
      LightPosition[0] += rx*0.01f;
      lighting_needs_update = GL_TRUE;
   }

   if (abs(ry) > ANALOGS_DEADZONE) {
      LightPosition[1] -= ry*0.01f;
      lighting_needs_update = GL_TRUE;
   }

   if (lighting_needs_update) glUniform3fv(LightPosition_location, 1, LightPosition);
}


void gl_init() {
   vglInitExtended(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x800000, SCE_GXM_MULTISAMPLE_4X);
   sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE); // Enabling sampling for the analogs
   glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CW); 
   glClearColor(0.1, 0.1, 0.1, 1.0);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


GLuint cube_program;
GLuint cube_vertex_buffer_id, cube_texture_id;

static void cube_init(void) {
   cube_program = glCreateProgram();
   load_shader("app0:shader/tetromino.vert.cg", &cube_program);
   load_shader("app0:shader/tetromino.frag.cg", &cube_program);

   glBindAttribLocation(cube_program, POSITION_LOCATION, "position");
   glBindAttribLocation(cube_program, TEXCOORD_LOCATION, "texcoord");
   glBindAttribLocation(cube_program, TYPE_LOCATION,      "type");

   glLinkProgram(cube_program);
   glUseProgram(cube_program);

   ViewMatrix_location              = glGetUniformLocation(cube_program, "ViewMatrix");
   ModelMatrix_location             = glGetUniformLocation(cube_program, "ModelMatrix");
   LightPosition_location           = glGetUniformLocation(cube_program, "LightPosition");
   GLuint ProjectionMatrix_location = glGetUniformLocation(cube_program, "ProjectionMatrix");
   GLuint FaceTypeNormals_location  = glGetUniformLocation(cube_program, "FaceTypeNormals");

   glUniformMatrix4fv(ViewMatrix_location, 1, GL_FALSE, ViewMatrix);
   glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, ModelMatrix);
   glUniform3fv(LightPosition_location, 1, LightPosition);

   float ProjectionMatrix[16];
   perspective(ProjectionMatrix, 60, ((float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT), 1, 1024);
   glUniformMatrix4fv(ProjectionMatrix_location, 1, GL_FALSE, ProjectionMatrix);
   
   const float FaceTypeNormals[FACE_QUANTITY][3] = {[FACE_FRONT]  = {0,  0, 1},
                                                    [FACE_TOP]    = {0,  1, 0},
                                                    [FACE_RIGHT]  = {1,  0, 0},
                                                    [FACE_BOTTOM] = {0, -1, 0},
                                                    [FACE_LEFT]   = {-1, 0, 0}};
   glUniform3fv(FaceTypeNormals_location, FACE_QUANTITY, (const float*)FaceTypeNormals);

   parse_playfield_to_triangles();

   glGenBuffers(1, &cube_vertex_buffer_id);
   glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer_id);
   glBufferData(/* type */  GL_ARRAY_BUFFER,
                /* size */  PLAYFIELD_VERTEX_BUFFER_SIZE * sizeof(tetromino_vertex_t),
                /* data */  PLAYFIELD_VERTEX_BUFFER,
                /* usage */ GL_STATIC_DRAW);

   /* Load block image texture and bind to openGL */
   unsigned int texture_width, texture_height, texture_channels;
   uint8_t *texture_pixels = stbi_load("app0:texture/block.bmp",
                                       &texture_width,
                                       &texture_height,
                                       &texture_channels,
                                       0);
   flip_rgb_image_vertically(texture_pixels, texture_width, texture_height);
      
   glGenTextures(1, &cube_texture_id);
   glBindTexture(GL_TEXTURE_2D, cube_texture_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RGB,
                /* width */  texture_width,
                /* height */ texture_height,
                /* border */ 0,
                /* format */ GL_RGB,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   texture_pixels);

   stbi_image_free(texture_pixels);
   // free(texture_pixels);

   glUniform1i(glGetUniformLocation(cube_program, "gTexture"), 0);
   glActiveTexture(GL_TEXTURE0);
}


static void cube_draw() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer_id);
   glUseProgram(cube_program);

   glEnableVertexAttribArray(POSITION_LOCATION);
   glVertexAttribPointer(/* location */  POSITION_LOCATION,
                         /* dimension */ 3,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(tetromino_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(tetromino_vertex_t,x));
                                    // If GL_ARRAY_BUFFER is bound, *pointer* is an offset into it

   glEnableVertexAttribArray(TEXCOORD_LOCATION);
   glVertexAttribPointer(/* location */  TEXCOORD_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(tetromino_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(tetromino_vertex_t,u));

   glEnableVertexAttribArray(TYPE_LOCATION);
   glVertexAttribPointer(/* location */  TYPE_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(tetromino_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(tetromino_vertex_t,block));

   glDrawArrays(/*mode=*/GL_TRIANGLES, /*first=*/0, /*count=*/PLAYFIELD_VERTEX_BUFFER_SIZE);

   glDisableVertexAttribArray(POSITION_LOCATION);
   glDisableVertexAttribArray(TEXCOORD_LOCATION);
   glDisableVertexAttribArray(TYPE_LOCATION);
   glUseProgram(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   vglSwapBuffers(GL_FALSE);
}


typedef struct {
    int16_t x, y, z;
    uint8_t u, v;
} background_vertex_t;

#define BACKGROUND_Z -10
const background_vertex_t BACKGROUND_VERTEX_BUFFER[] = {
   (background_vertex_t){.x=-DISPLAY_WIDTH/2, .y=-DISPLAY_HEIGHT/2, .z=BACKGROUND_Z, .u=0, .v=0},
   (background_vertex_t){.x=-DISPLAY_WIDTH/2, .y=DISPLAY_HEIGHT/2,  .z=BACKGROUND_Z, .u=0, .v=1},
   (background_vertex_t){.x=DISPLAY_WIDTH/2,  .y=-DISPLAY_HEIGHT/2, .z=BACKGROUND_Z, .u=1, .v=0},
   (background_vertex_t){.x=DISPLAY_WIDTH/2,  .y=DISPLAY_HEIGHT/2,  .z=BACKGROUND_Z, .u=1, .v=1},
};

// TO-DO: Show background at z=0 without depth test enabled

GLuint background_program, background_vertex_buffer_id, background_texture_id;
void background_init() {
   glLinkProgram(background_program);
   glUseProgram(background_program);

   glGenBuffers(1, &background_vertex_buffer_id);
   glBindBuffer(GL_ARRAY_BUFFER, background_vertex_buffer_id);
   glBufferData(/* type */  GL_ARRAY_BUFFER,
                /* size */  sizeof(BACKGROUND_VERTEX_BUFFER),
                /* data */  BACKGROUND_VERTEX_BUFFER,
                /* usage */ GL_STATIC_DRAW);

   unsigned int texture_width, texture_height, texture_channels;
   uint8_t *texture_pixels = stbi_load("app0:texture/bg.bmp",
                                       &texture_width,
                                       &texture_height,
                                       &texture_channels,
                                       0);
   flip_rgb_image_vertically(texture_pixels, texture_width, texture_height);
      
   glGenTextures(1, &background_texture_id);
   glBindTexture(GL_TEXTURE_2D, background_texture_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RGB,
                /* width */  texture_width,
                /* height */ texture_height,
                /* border */ 0,
                /* format */ GL_RGB,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   texture_pixels);

   stbi_image_free(texture_pixels);
   // free(texture_pixels);

   glUniform1i(glGetUniformLocation(cube_program, "gTexture"), 0);
   glActiveTexture(GL_TEXTURE0);
}


int main(int argc, char *argv[]) {
   gl_init();

   cube_init();

   while(1) {
    read_input();
    cube_draw();
    // cube_idle();
   }

   return 0;
}
