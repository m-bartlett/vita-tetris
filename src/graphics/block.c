#include <stdint.h>
#include "block.h"
#include "../engine/playfield.h"
#include "../engine/tetromino.h"
#include "../lib/linalg.h"
#include "../lib/bmp.h"

#define TEXTURE_PATH "texture/block.bmp"
#define VERTEX_SHADER_PATH "shader/block.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/block.frag.cg"

enum vertex_attribute_location { VERTEX_ATTRIBUTE_POSITION_LOCATION,
                                 VERTEX_ATTRIBUTE_TEXCOORD_LOCATION,
                                 VERTEX_ATTRIBUTE_TYPE_LOCATION };

/*
    TO-DO: Implement #ifdef USE_SINGLE_TEXTURE_OBJ
*/
static GLuint program;
static GLuint texture_id;
static GLuint u_view_matrix_location,
              u_model_matrix_location,
              u_projection_matrix_location,
              u_light_position_location;

// TO-DO: Try adjusting perspective matrix so Z_INITIAL_OFFSET can be 0
#define Z_INITIAL_OFFSET -20
static GLfloat u_view_matrix[16] = { [0] = 1, [5] = 1, [10] = 1, [15] = 1,
                                     [12]=-PLAYFIELD_WIDTH/2,
                                     [13]=-PLAYFIELD_HEIGHT/2,
                                     [14]=Z_INITIAL_OFFSET, };
static GLfloat u_light_position[3] = { 0.0, 0.0, 100.0};

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


void graphics_block_init(void)
{ //{{{
    program = glCreateProgram();
    load_shader("app0:" VERTEX_SHADER_PATH, &program);
    load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

    glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");
    glBindAttribLocation(program, VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, "texture_coordinate");
    glBindAttribLocation(program, VERTEX_ATTRIBUTE_TYPE_LOCATION,     "type");

    glLinkProgram(program);
    glUseProgram(program);


    u_view_matrix_location              = glGetUniformLocation(program, "u_view_matrix");
    u_model_matrix_location             = glGetUniformLocation(program, "u_model_matrix");
    u_light_position_location           = glGetUniformLocation(program, "u_light_position");
    GLuint u_projection_matrix_location = glGetUniformLocation(program, "u_projection_matrix");
    GLuint u_face_type_normals_location = glGetUniformLocation(program, "u_face_type_normals");
    GLuint u_block_type_colors_location = glGetUniformLocation(program, "u_block_type_colors");

    glUniformMatrix4fv(u_view_matrix_location, 1, GL_FALSE, u_view_matrix);
    glUniform3fv(u_light_position_location, 1, u_light_position);
    glUniformMatrix4fv(/*location=*/u_projection_matrix_location,
                       /*count=*/1,
                       /*tranpose=*/GL_FALSE,
                       /*value=*/(float[])\
                                 perspective_matrix(60,
                                                    DISPLAY_WIDTH/(float)DISPLAY_HEIGHT,
                                                    1,
                                                    1024));
   
    const float u_face_type_normals[FACE_QUANTITY][3] = {[FACE_FRONT]  = {0,  0, 1},
                                                         [FACE_TOP]    = {0,  1, 0},
                                                         [FACE_RIGHT]  = {1,  0, 0},
                                                         [FACE_BOTTOM] = {0, -1, 0},
                                                         [FACE_LEFT]   = {-1, 0, 0}};
    glUniform3fv(u_face_type_normals_location, FACE_QUANTITY, (const float*)u_face_type_normals);

    // TO-DO: 3 bytes instead of 3 floats, maybe try normalize=true or /255.0 in shader
    const float u_block_type_colors[TETROMINO_TYPE_QUANTITY][3] = {
        [TETROMINO_TYPE_NULL] = {1.0, 1.0, 1.0},
        [TETROMINO_TYPE_I] = {0.2431, 0.8627, 1.0},
        [TETROMINO_TYPE_O] = {1.0,    0.7750, 0.1216},
        [TETROMINO_TYPE_T] = {0.7920, 0.1726, 0.6745},
        [TETROMINO_TYPE_J] = {0.1255, 0.3333, 0.8314},
        [TETROMINO_TYPE_L] = {1.0,    0.406,  0.029},
        [TETROMINO_TYPE_S] = {0.4980, 0.8275, 0.0824},
        [TETROMINO_TYPE_Z] = {0.9333, 0.102,  0.2118},
    };
    glUniform3fv(u_block_type_colors_location,
                 TETROMINO_TYPE_QUANTITY,
                 (const float*)u_block_type_colors);

    load_texture();

    glUniform1i(glGetUniformLocation(program, "u_block_texture"), 0);
/*}}}*/ }


void graphics_block_end(void)
{ //{{{
    glDeleteProgram(program);
    glDeleteTextures(/*texture_quantity=*/1, &texture_id);
/*}}}*/ }


void graphics_block_set_model_matrix(const float model_matrix[16]) {
    glUniformMatrix4fv(u_model_matrix_location, 1, GL_FALSE, (GLfloat*)model_matrix);
}


void graphics_block_draw(GLuint vertex_buffer_id)
{ //{{{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    unsigned int vertex_buffer_size;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vertex_buffer_size);
    vertex_buffer_size /= sizeof(graphics_block_vertex_t);

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
    glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_TYPE_LOCATION,  /* a.k.a. index */
                          /* dimension */ 2,                               /* a.k.a. size */
                          /* type */      GL_UNSIGNED_BYTE,
                          /* normalize */ GL_FALSE,
                          /* stride */    sizeof(graphics_block_vertex_t),
                          /* pointer */   (GLvoid*)offsetof(graphics_block_vertex_t,block));

    glDrawArrays(/*mode=*/GL_QUADS, /*first=*/0, /*count=*/vertex_buffer_size);

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
    /* Prepare face vertices to be drawn as GL_QUADs */
    const uint8_t x1=x+1, y1=y+1;

#define ADD_VERTEX(...) \
    vertex_buffer[(*vertex_buffer_size)++] = (graphics_block_vertex_t){__VA_ARGS__}

    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_FRONT);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_FRONT);

    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=0, .v=1, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=0, .v=2, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_LEFT);
    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_LEFT);

    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=3, .v=2, .block=block_type, .face=FACE_RIGHT);
    ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=3, .v=1, .block=block_type, .face=FACE_RIGHT);

    ADD_VERTEX(.x=x,  .y=y1, .z=1, .u=1, .v=2, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x,  .y=y1, .z=0, .u=1, .v=3, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x1, .y=y1, .z=0, .u=2, .v=3, .block=block_type, .face=FACE_TOP);
    ADD_VERTEX(.x=x1, .y=y1, .z=1, .u=2, .v=2, .block=block_type, .face=FACE_TOP);

    ADD_VERTEX(.x=x,  .y=y,  .z=0, .u=1, .v=0, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x,  .y=y,  .z=1, .u=1, .v=1, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x1, .y=y,  .z=1, .u=2, .v=1, .block=block_type, .face=FACE_BOTTOM);
    ADD_VERTEX(.x=x1, .y=y,  .z=0, .u=2, .v=0, .block=block_type, .face=FACE_BOTTOM);

/*}}}*/}