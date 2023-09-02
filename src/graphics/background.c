/*
    TO-DO: Implement #ifdef USE_SINGLE_TEXTURE_OBJ
*/

#include <stdint.h>
#include "core.h"
#include "background.h"
#include "../lib/bmp.h"

#define TEXTURE_PATH         "texture/background.bmp"
#define VERTEX_SHADER_PATH   "shader/background.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/background.frag.cg"

enum vertex_attribute_location { VERTEX_ATTRIBUTE_POSITION_LOCATION,
                                 VERTEX_ATTRIBUTE_TEXCOORD_LOCATION };

static GLuint program;
static GLuint vertex_buffer_id;
static GLuint texture_id;

typedef struct { int8_t x, y; uint8_t u, v; } vertex_t;

static const vertex_t
vertex_buffer[] = { (vertex_t){.x=-1, .y=-1, .u=0, .v=0},
                    (vertex_t){.x=-1, .y= 1, .u=0, .v=1},
                    (vertex_t){.x= 1, .y=-1, .u=1, .v=0},
                    (vertex_t){.x= 1, .y= 1, .u=1, .v=1} };


static void load_texture() { //{{{
    unsigned int width, height;
    uint8_t *pixels = read_rgb_bmp_image("app0:" TEXTURE_PATH, &width, &height);
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
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
    free(pixels);
/*}}}*/ }


void graphics_background_init(void) { //{{{
    program = glCreateProgram();
    load_shader("app0:" VERTEX_SHADER_PATH, &program);
    load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

    glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");
    glBindAttribLocation(program, VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, "texcoord");

    glLinkProgram(program);
    glUseProgram(program);

    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/*type*/ GL_ARRAY_BUFFER,
                 /*size*/ sizeof(vertex_buffer),
                 /*data*/ vertex_buffer,
                 /*usage*/GL_STATIC_DRAW);

    load_texture();

    glUniform1i(glGetUniformLocation(program, "gTexture"), 1);
/*}}}*/ }


void graphics_background_end(void) { //{{{
    glDeleteProgram(program);
    glDeleteTextures(/*texture_quantity=*/1, &texture_id);
    glDeleteBuffers(/*buffer_quantity=*/1, &vertex_buffer_id);
/*}}}*/ }


void graphics_background_draw(void) { //{{{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);

   // TO-DO: For single texture object, use GL_TEXTURE1

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
    glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_POSITION_LOCATION,
                          /* dimension */ 2,
                          /* type */      GL_BYTE,
                          /* normalize */ GL_FALSE,
                          /* stride */    sizeof(vertex_t),
                          /* pointer */   (GLvoid*)offsetof(vertex_t,x));

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
    glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_TEXCOORD_LOCATION,
                          /* dimension */ 2,
                          /* type */      GL_UNSIGNED_BYTE,
                          /* normalize */ GL_FALSE,
                          /* stride */    sizeof(vertex_t),
                          /* pointer */   (GLvoid*)offsetof(vertex_t,u));


    glDisable(GL_DEPTH_TEST);
    glDrawArrays(/*mode=*/GL_TRIANGLE_STRIP,
                 /*first=*/0,
                 /*count=*/ARRAY_SIZE(vertex_buffer));
    glEnable(GL_DEPTH_TEST);


    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }