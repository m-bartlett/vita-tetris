#include <stdint.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"

// 6 vertices for each face of 5 visible faces for each of 4 blocks per tetromino
#define VERTEX_COUNT_MAX (6*5*4) 

static GLuint vertex_buffer_id;
static graphics_block_vertex_t vertex_buffer[VERTEX_COUNT_MAX];
static uint32_t vertex_buffer_size;


void graphics_tetromino_init(void)
{ //{{{
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  sizeof(vertex_buffer),
                 /* data */  vertex_buffer,
                 /* usage */ GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


void graphics_tetromino_end(void)
{ //{{{
    glDeleteBuffers(/*buffer_quantity=*/1, &vertex_buffer_id);
/*}}}*/ }


void graphics_tetromino_to_block_vertices(const tetromino_t *t)
{ //{{{
    vertex_buffer_size = 0;
    const uint8_t B = t->type;
    const uint16_t grid = tetromino_get_grid(t);
    uint16_t maskbit = (uint16_t)1<<15;
    for (int8_t y = 0; y < 4; ++y) {
        for (int8_t x = 0; x < 4; ++x) {
            if (maskbit&grid)
                graphics_block_add_block_to_vertex_buffer(x,
                                                          y,
                                                          B,
                                                          vertex_buffer,
                                                          &vertex_buffer_size);
            maskbit >>=1;
        }
    }
/*}}}*/ }


void graphics_tetromino_draw_buffer()
{ //{{{
    graphics_block_draw(vertex_buffer_id, vertex_buffer_size);
/*}}}*/ }


void graphics_tetromino_draw(const tetromino_t *t)
{ //{{{
    graphics_tetromino_to_block_vertices(t);
    graphics_tetromino_draw_buffer();
/*}}}*/ }