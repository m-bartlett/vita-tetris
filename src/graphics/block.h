/********************************************************************************
 **                                                                            **
 **  GL resource initialization and abstraction for drawing individual blocks. **
 **  These are mutually used by playfield and individual tetromino renderers.  **
 **                                                                            **
 ********************************************************************************/

#ifndef GRAPHICS_BLOCK_H
#define GRAPHICS_BLOCK_H

#include "core.h"

enum face_type_enum { FACE_FRONT  = 0,
                      FACE_TOP    = 1,
                      FACE_RIGHT  = 2,
                      FACE_BOTTOM = 3,
                      FACE_LEFT   = 4,
                      FACE_QUANTITY };

typedef enum face_type_enum face_type_t;

typedef struct { uint8_t x, y, z;
                 uint8_t u, v;
                 uint8_t block, face; } graphics_block_vertex_t;

// ROM meshes are indexed by tetromino type, so we don't need to redundantly store that data
typedef struct { uint8_t x, y, z;
                 uint8_t u, v, face; } graphics_block_ROM_vertex_t;

void graphics_block_init(void);
void graphics_block_end(void);
void graphics_block_set_model_matrix_2D(float x, float y);
void graphics_block_set_model_matrix_3D(float x,  float y,  float z,
                                        float cx, float cy, float cz,
                                        float theta, float phi);
void graphics_block_set_model_matrix(const float ModelMatrix[16]);
void graphics_block_draw(GLuint vertex_buffer_id, unsigned int vertex_buffer_size);
void graphics_block_draw_alpha(GLuint vertex_buffer_id, unsigned int vertex_buffer_size);
void graphics_block_add_block_to_vertex_buffer(uint8_t x,
                                               uint8_t y,
                                               uint8_t block_type,
                                               graphics_block_vertex_t *vertex_buffer,
                                               uint32_t *vertex_buffer_size);

/*
    TO-DO: Tetronimo colors are a uniform array, the block-type enum indexes into the array
*/

#endif