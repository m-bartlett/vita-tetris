#include <stdint.h>
#include "core.h"
#include "block.h"
#include "playfield.h"
#include "../engine/playfield.h"

// 6 vertices for each face of 5 visible faces for each block in the playfield grid
#define VERTEX_COUNT_MAX (6*5*PLAYFIELD_HEIGHT*PLAYFIELD_WIDTH) 


static GLuint vertex_buffer_id;
static graphics_block_vertex_t vertex_buffer[VERTEX_COUNT_MAX];
static uint32_t vertex_buffer_size;

void graphics_playfield_convert_grid_to_block_vertices(void)
{ //{{{
    uint8_t y=0, x=0, y1=0, x1=0, r=PLAYFIELD_HEIGHT-1;
    playfield_view_t playfield = playfield_view();
    vertex_buffer_size = 0;
    
    while (y < PLAYFIELD_HEIGHT) {
        y1 = y+1;
        x = 0;
        const char* row = playfield[r--];

        while (x < PLAYFIELD_WIDTH) {
            const uint8_t B = row[x];
            x1 = x+1;
            if (B != 0)
                graphics_block_add_block_to_vertex_buffer(x,
                                                          y,
                                                          B,
                                                          vertex_buffer,
                                                          &vertex_buffer_size);
            x = x1;
        }
        y = y1;
    }
/*}}}*/ }


void graphics_playfield_init(void)
{ //{{{
    graphics_playfield_convert_grid_to_block_vertices();

    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  ARRAY_SIZE(vertex_buffer),
                 /* data */  vertex_buffer,
                 /* usage */ GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


void graphics_playfield_end(void)
{ //{{{
    glDeleteBuffers(/*buffer_quantity=*/1, &vertex_buffer_id);
/*}}}*/ }


void graphics_playfield_draw(void)
{ //{{{
    graphics_block_draw(vertex_buffer_id, vertex_buffer_size);
/*}}}*/ }


void graphics_playfield_draw_update(void)
{ //{{{
    graphics_playfield_convert_grid_to_block_vertices();
    graphics_playfield_draw();
/*}}}*/ }