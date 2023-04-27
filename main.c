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


// static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
//  {1,0,0,0,0,0,0,0,0,3},
//  {2,3,0,0,0,0,0,0,0,0},
//  {4,0,1,0,0,0,0,0,0,0},
//  {7,1,1,3,0,0,0,0,0,0}, 
//  {4,0,6,0,1,0,0,0,0,0},
//  {2,3,0,5,6,7,0,0,0,0},
//  {0,1,2,3,0,5,6,0,0,0},
//  {0,0,7,1,2,3,4,5,0,0},
//  {0,0,0,6,7,1,2,3,4,0},
//  {0,0,0,0,5,7,6,1,2,3},
//  {0,5,6,7,7,7,0,0,0,0},
//  {1,3,0,5,6,7,1,0,0,0},
//  {0,1,0,3,0,5,0,7,0,0},
//  {0,0,0,2,0,4,0,6,0,3},
//  {0,0,0,0,7,1,2,3,4,5},
//  {0,0,0,0,0,6,7,1,2,3},
//  {0,0,0,0,0,0,4,5,6,7},
//  {0,0,0,0,0,0,0,1,2,3},
//  {0,0,0,0,0,0,0,0,4,5},
//  {0,0,0,0,0,0,0,0,0,6},
// };


static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
    {0,7,3,0,7,0,0,0,7,6},
    {0,4,6,0,7,6,1,0,6,5},
    {0,3,6,5,7,2,0,0,5,4},
    {0,6,5,4,7,2,0,0,6,2},
    {0,3,4,3,5,0,0,0,5,7},
    {0,2,1,7,3,2,2,1,5,7},
    {0,1,4,6,2,1,7,6,1,7},
    {0,1,3,5,0,7,2,5,1,7},
    {5,4,7,6,7,6,2,4,6,5},
    {5,4,6,5,0,0,4,3,6,3},
    {6,5,7,2,5,5,3,2,5,4},
    {2,2,5,2,4,3,6,1,4,3},
    {2,1,3,2,2,1,3,7,3,2},
    {1,7,3,2,7,6,2,5,7,6},
    {2,0,6,1,1,7,6,1,7,6},
    {5,0,1,3,6,5,7,4,1,7},
    {5,0,4,7,6,5,4,7,2,4},
    {3,0,6,4,6,5,4,0,5,1},
    {4,0,3,5,7,3,2,0,2,5},
    {4,0,2,1,5,7,2,0,4,6},
};


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
   char shader_body[shader_size+1];
   fread(shader_body, 1, shader_size, f);
   fclose(f);
   shader_body[shader_size] = 0;
   printf("%s\n\n", shader_body);

   GLuint shader_ref = glCreateShader(shader_type);
   glShaderSource(/* shader */ shader_ref,
                  /* sources count */ 1,
                  /* array of string arrays */ (const char*[]){(const char*)&shader_body},
                  /* string length(s) */ &shader_size);
   glCompileShader(shader_ref);
   glAttachShader(*program, shader_ref);
}


static inline void rgb_image_flip_vertically(uint8_t* buffer, uint16_t width, uint16_t height) {
    /* Flip the image vertically, OpenGL expects (0,0) to be the bottom-left of the image */
   const uint32_t row_size = width * 3;
   uint32_t top_row_index = 0;
   uint32_t bottom_row_index = (height - 1) * row_size;
   uint8_t temp_row[row_size];
   for (int y = 0; y < height / 2; y++) {
      memcpy(temp_row, &buffer[top_row_index], row_size);
      memcpy(&buffer[top_row_index], &buffer[bottom_row_index], row_size);
      memcpy(&buffer[bottom_row_index], temp_row, row_size);
      top_row_index += row_size;
      bottom_row_index -= row_size;
   }
}


static inline void rgb_image_to_grayscale(uint8_t* buffer, uint16_t width, uint16_t height) {
   const uint32_t row_size = width * 3;
   const uint32_t pixel_count = row_size * height;
   uint32_t i = 0;
   for (uint32_t y = 0; y < pixel_count; y+=row_size) {
      for (uint32_t x = 0; x < row_size; x+=3) {
         uint32_t _x = y+x;
         uint32_t r = buffer[_x], g = buffer[_x+1], b = buffer[_x+2];
         uint8_t magnitude = sqrt((r*r) + (g*g) + (b*b)) * 255 / sqrt((255*255)*3);
         // uint8_t magnitude = (r+g+b)/sqrt(13);
         buffer[i++] = magnitude;
      }
   }
}


static const uint8_t quarter_sine_lookup[64] = {
   /* [round(255*(-math.cos(x)/2+0.5)) for x in np.linspace(0,math.pi/2,64)] */
   0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 16, 17, 19, 20, 22, 24, 26, 28,
   30, 32, 34, 36, 38, 41, 43, 46, 48, 51, 53, 56, 58, 61, 64, 67, 69, 72, 75, 78, 81, 84, 87, 90,
   93, 96, 99, 102, 105, 108, 112, 115, 118, 121, 124, 127
};
static uint8_t byte_sine (uint8_t x) {
   switch (x>>6) {
      default:   // for uint8_t, ~x == 255-x
      case 0: return quarter_sine_lookup[x];
      case 1: return ~quarter_sine_lookup[(~x)&63];
      case 2: return ~quarter_sine_lookup[x&63];
      case 3: return quarter_sine_lookup[(~x)&63];
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


uint32_t ticks=0;
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

   // if (pad.buttons & SCE_CTRL_LEFT)  { user_offset[1] += 5.0; }
   // if (pad.buttons & SCE_CTRL_RIGHT) { user_offset[1] -= 5.0; }
   if (pad.buttons & SCE_CTRL_UP)    { user_offset[0] += 5.0; }
   if (pad.buttons & SCE_CTRL_DOWN)  { user_offset[0] -= 5.0; }

   // double t = (double)sceKernelGetProcessTimeWide() / 1000000.0;
   int16_t tick_sine = byte_sine((ticks++>>1) & 0xff) - 127;
   double angle = (tick_sine * M_PI)/4095.0;

   user_offset[1] = angle;

   identity(ModelMatrix);
   translate(ModelMatrix, PLAYFIELD_WIDTH/2, PLAYFIELD_HEIGHT/2, 0);
   rotate(ModelMatrix, 2 * M_PI * user_offset[0] / 360.0, 1, 0, 0);
   // rotate(ModelMatrix, 2 * M_PI * user_offset[1] / 360.0, 0, 1, 0);
   rotate(ModelMatrix, user_offset[1], 0, 1, 0);
   translate(ModelMatrix, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, 0);
   glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, ModelMatrix);

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

GLuint texture_id;

void texture_init() {
   /* Load block image texture and bind to openGL */
   unsigned int width, height, channels;
   uint8_t *pixels;

   pixels = stbi_load("app0:texture/block2.bmp", &width, &height, &channels, 0);
   rgb_image_flip_vertically(pixels, width, height);
   rgb_image_to_grayscale(pixels, width, height);
      
   glGenTextures(1, &texture_id);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RED,
                /* width */  width,
                /* height */ height,
                /* border */ 0,
                /* format */ GL_RED,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   pixels);
   stbi_image_free(pixels);


   pixels = stbi_load("app0:texture/bg.bmp", &width, &height, &channels, 0);
   rgb_image_flip_vertically(pixels, width, height);
      
   glGenTextures(1, &texture_id);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RGB,
                /* width */  width,
                /* height */ height,
                /* border */ 0,
                /* format */ GL_RGB,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   pixels);
   stbi_image_free(pixels);
}


//--------------------------------------- tetromino ----------------------------------------------//

GLuint cube_program;
GLuint cube_vertex_buffer_id;
GLuint cube_texture_id;

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

   glUniform1i(glGetUniformLocation(cube_program, "gTexture"), 0);
}


static void cube_draw() {
   glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer_id);
   glUseProgram(cube_program);
   // glBindTexture(GL_TEXTURE_2D, texture_id);
   glActiveTexture(GL_TEXTURE0);

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
}






//-----------------------------------------background---------------------------------------------//



typedef struct { int8_t x, y; uint8_t u, v; } background_vertex_t;

const background_vertex_t BACKGROUND_VERTEX_BUFFER[] = {
   // (background_vertex_t){.x=-DISPLAY_WIDTH/2, .y=-DISPLAY_HEIGHT/2, .u=0, .v=0},
   // (background_vertex_t){.x=-DISPLAY_WIDTH/2, .y=DISPLAY_HEIGHT/2,  .u=0, .v=1},
   // (background_vertex_t){.x=DISPLAY_WIDTH/2,  .y=-DISPLAY_HEIGHT/2, .u=1, .v=0},
   // (background_vertex_t){.x=DISPLAY_WIDTH/2,  .y=DISPLAY_HEIGHT/2,  .u=1, .v=1},
   (background_vertex_t){.x=-1, .y=-1, .u=0, .v=0},
   (background_vertex_t){.x=-1, .y=1,  .u=0, .v=1},
   (background_vertex_t){.x=1,  .y=-1, .u=1, .v=0},
   (background_vertex_t){.x=1,  .y=1,  .u=1, .v=1},
};

GLuint background_program;
GLuint background_vertex_buffer_id;

void background_init() {
   background_program = glCreateProgram();
   load_shader("app0:shader/bg.vert.cg", &background_program);
   load_shader("app0:shader/bg.frag.cg", &background_program);

   glBindAttribLocation(background_program, POSITION_LOCATION, "position");
   glBindAttribLocation(background_program, TEXCOORD_LOCATION, "texcoord");

   glLinkProgram(background_program);
   glUseProgram(background_program);

   glGenBuffers(1, &background_vertex_buffer_id);
   glBindBuffer(GL_ARRAY_BUFFER, background_vertex_buffer_id);
   glBufferData(/*type*/ GL_ARRAY_BUFFER,
                /*size*/ sizeof(BACKGROUND_VERTEX_BUFFER),
                /*data*/ BACKGROUND_VERTEX_BUFFER,
                /*usage*/GL_STATIC_DRAW);

   glUniform1i(glGetUniformLocation(background_program, "gTexture"), 1);
}


void background_draw() {
   glBindBuffer(GL_ARRAY_BUFFER, background_vertex_buffer_id);
   glUseProgram(background_program);
   // glBindTexture(GL_TEXTURE_2D, texture_id);
   glActiveTexture(GL_TEXTURE1);

   glEnableVertexAttribArray(POSITION_LOCATION);
   glVertexAttribPointer(/* location */  POSITION_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(background_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(background_vertex_t,x));

   glEnableVertexAttribArray(TEXCOORD_LOCATION);
   glVertexAttribPointer(/* location */  TEXCOORD_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(background_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(background_vertex_t,u));


   glDisable(GL_DEPTH_TEST);
   glDrawArrays(/*mode=*/GL_TRIANGLE_STRIP,
                /*first=*/0,
                /*count=*/ARRAY_SIZE(BACKGROUND_VERTEX_BUFFER));
   glEnable(GL_DEPTH_TEST);


   glDisableVertexAttribArray(POSITION_LOCATION);
   glDisableVertexAttribArray(TEXCOORD_LOCATION);
   glUseProgram(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int main(int argc, char *argv[]) {
   gl_init();

   texture_init();
   background_init();
   cube_init();

   while(1) {
      read_input();
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      background_draw();
      cube_draw();
      vglSwapBuffers(GL_FALSE);
   }

   return 0;
}
