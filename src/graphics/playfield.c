#include <stdint.h>
#include "core.h"
#include "block.h"
#include "playfield.h"
#include "../engine/playfield.h"
#include "../engine/tetromino.h"

// 4 vertices for each face of 5 visible faces for each block in the playfield grid
#define VERTEX_COUNT_MAX (4*5*PLAYFIELD_HEIGHT*PLAYFIELD_WIDTH*100)


static GLuint vertex_buffer_id;
static graphics_block_vertex_t vertex_buffer[VERTEX_COUNT_MAX]={{0}};
static uint32_t vertex_buffer_size;

void graphics_playfield_update_mesh(void)
{ //{{{
    /* convert playfield grid blocks to block vertices */
    uint8_t y=0, x=0, r=PLAYFIELD_HEIGHT-1;
    playfield_view_t playfield = playfield_view();
    vertex_buffer_size = 0;
    
    while (y < PLAYFIELD_HEIGHT) {
        x = 0;
        const int8_t* row = playfield[r--];

        while (x < PLAYFIELD_WIDTH) {
            uint8_t block_type = (uint8_t)row[x];
            if (block_type != TETROMINO_TYPE_NULL)
                graphics_block_add_block_to_vertex_buffer(x,
                                                          y,
                                                          block_type,
                                                          vertex_buffer,
                                                          &vertex_buffer_size);
            ++x;
        }
        ++y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  sizeof(graphics_block_vertex_t)*vertex_buffer_size,
                 /* data */  vertex_buffer,
                 /* usage */ GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


void graphics_playfield_init(void)
{ //{{{
    graphics_playfield_update_mesh();
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  sizeof(vertex_buffer),
                 /* data */  vertex_buffer,
                 /* usage */ GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


void graphics_playfield_end(void)
{ //{{{
    glDeleteBuffers(/*buffer_quantity=*/1, &vertex_buffer_id);
/*}}}*/ }


static const float model_matrix[16] = { [0]=1, [5]=1, [10]=1, [15]=1 };
void graphics_playfield_draw(void)
{ //{{{
    graphics_block_set_model_matrix(model_matrix);
    graphics_block_draw(vertex_buffer_id, vertex_buffer_size);
/*}}}*/ }