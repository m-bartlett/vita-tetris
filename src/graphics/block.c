#include <stdint.h>
#include "block.h"
#include "../engine/playfield.h"
#include "../lib/linalg.h"
#include "../lib/bmp.h"

#define TEXTURE_PATH "texture/block.bmp"
#define VERTEX_SHADER_PATH "shader/block.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/block.frag.cg"

#define VERTEX_ATTRIBUTE_POSITION_LOCATION 0
#define VERTEX_ATTRIBUTE_TEXCOORD_LOCATION 1
#define VERTEX_ATTRIBUTE_TYPE_LOCATION 2


/*
    TO-DO: Implement #ifdef USE_SINGLE_TEXTURE_OBJ
*/
static GLuint program;
static GLuint texture_id;

static GLuint ViewMatrix_location,
              ModelMatrix_location,
              ProjectionMatrix_location,
              LightPosition_location;

#define Z_INITIAL_OFFSET -20
// static GLfloat user_offset[3] = { 0.0, 0.0, Z_INITIAL_OFFSET };
static GLfloat ViewMatrix[16] = { [0] = 1, [5] = 1, [10] = 1, [15] = 1,
                                  [12]=-PLAYFIELD_WIDTH/2,
                                  [13]=-PLAYFIELD_HEIGHT/2,
                                  [14]=Z_INITIAL_OFFSET, };
static GLfloat ModelMatrix[16] = { [0]=1, [5]=1, [10]=1, [15]=1 };
static GLfloat LightPosition[3] = { 0.0, 0.0, 100.0};

/* TO-DO: Un-CameCase these uniform names */


void graphics_block_set_model_matrix_3D(float x,  float y,  float z,
                                        float cx, float cy, float cz,
                                        float theta, float phi)
{ //{{{
    identity(ModelMatrix);
    translate(ModelMatrix, cx, cy, 0);
    rotate(ModelMatrix, theta, 1, 0, 0);
    rotate(ModelMatrix, phi,   0, 1, 0);
    translate(ModelMatrix, -cx, -cy, 0);
    translate(ModelMatrix, x, y, 0);
    glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, ModelMatrix);
/*}}}*/ }



void graphics_block_set_model_matrix_2D(float x, float y)
{ //{{{
    identity(ModelMatrix);
    translate(ModelMatrix, x, y, 0);
    glUniformMatrix4fv(ModelMatrix_location, 1, GL_FALSE, ModelMatrix);
/*}}}*/ }

      
static void load_texture()
{ //{{{
    // Block texture is grayscale that's dynamically colored based on block type & user preferences
    unsigned int width, height;
    uint8_t *pixels = read_rgb_bmp_image_as_grayscale("app0:" TEXTURE_PATH, &width, &height);
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
    free(pixels);
/*}}}*/ }

/* TO-DO:
    Review which uniform locations need to be global and expose functions to modify static ones
*/

void graphics_block_init(void)
{ //{{{
   program = glCreateProgram();
   load_shader("app0:" VERTEX_SHADER_PATH, &program);
   load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

   glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");
   glBindAttribLocation(program, VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, "texcoord");
   glBindAttribLocation(program, VERTEX_ATTRIBUTE_TYPE_LOCATION,     "type");

   glLinkProgram(program);
   glUseProgram(program);


   ViewMatrix_location              = glGetUniformLocation(program, "ViewMatrix");
   ModelMatrix_location             = glGetUniformLocation(program, "ModelMatrix");
   LightPosition_location           = glGetUniformLocation(program, "LightPosition");
   GLuint ProjectionMatrix_location = glGetUniformLocation(program, "ProjectionMatrix");
   GLuint FaceTypeNormals_location  = glGetUniformLocation(program, "FaceTypeNormals");

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

   load_texture();

   glUniform1i(glGetUniformLocation(program, "gTexture"), 0);
/*}}}*/ }


void graphics_block_end(void)
{ //{{{
   glDeleteProgram(program);
   glDeleteTextures(/*texture_quantity=*/1, &texture_id);
/*}}}*/ }


void graphics_block_draw(GLuint vertex_buffer_id, unsigned int vertex_buffer_size)
{ //{{{
   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
   glUseProgram(program);
   glBindTexture(GL_TEXTURE_2D, texture_id); // TO-DO: Implement single texture object
   glActiveTexture(GL_TEXTURE0);

   glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
   glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_POSITION_LOCATION,
                         /* dimension */ 3,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(graphics_block_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(graphics_block_vertex_t,x));
                                    // If GL_ARRAY_BUFFER is bound, *pointer* is an offset into it

   glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
   glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_TEXCOORD_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(graphics_block_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(graphics_block_vertex_t,u));

   glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TYPE_LOCATION);
   glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_TYPE_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(graphics_block_vertex_t),
                         /* pointer */   (GLvoid*)offsetof(graphics_block_vertex_t,block));

   glDrawArrays(/*mode=*/GL_TRIANGLES, /*first=*/0, /*count=*/vertex_buffer_size);

   glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
   glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
   glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TYPE_LOCATION);
   glUseProgram(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


void graphics_block_add_block_to_vertex_buffer(uint8_t x,
                                               uint8_t y,
                                               uint8_t block_type,
                                               graphics_block_vertex_t *vertex_buffer,
                                               uint32_t *vertex_buffer_size)
{ //{{{
    const uint8_t x1=x+1, y1=y+1;

#define ADD_VERTEX(...) \
    vertex_buffer[(*vertex_buffer_size)++] = (graphics_block_vertex_t){__VA_ARGS__}

    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_FRONT);

    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=1, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=0, .v=2, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=1, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_LEFT);

    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=3, .v=2, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=3, .v=2, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=3, .v=1, .block=block_type, .face=FACE_RIGHT);

    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=1, .v=3, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=2, .v=3, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=2, .v=3, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_TOP);

    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=1, .v=0, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=1, .v=0, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=2, .v=0, .block=block_type, .face=FACE_BOTTOM);
    
/*}}}*/}