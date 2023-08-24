#include <stdint.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"
#include "tetromino_meshes.h"
#include "../engine/playfield.h"


// 6 vertices for each face of 5 visible faces for each of 4 blocks per tetromino
#define VERTEX_COUNT_MAX (6*5*4) 

// static GLuint vertex_buffer_id;
// static graphics_block_vertex_t vertex_buffer[VERTEX_COUNT_MAX];
static GLuint vertex_buffer_ids[TETROMINO_TYPE_QUANTITY][TETROMINO_ROTATION_QUANTITY];


void graphics_tetromino_init(void)
{ //{{{
    graphics_block_vertex_t vertex_buffer[TETROMINO_MEDIAN_MESH_SIZE];
    for (uint8_t t = 1; t < TETROMINO_TYPE_QUANTITY; ++t) {
        size_t tetromino_mesh_size = graphics_tetromino_get_mesh_size(t);

        for (int r = 0; r < TETROMINO_ROTATION_QUANTITY; ++r) {
            // tetromino_ROM_mesh = TETROMINO_MESHES[t][r];
            const graphics_block_ROM_vertex_t* 
                            tetromino_ROM_mesh = graphics_tetromino_get_mesh_ROM_vertex_buffer(t,r);
            for (unsigned int v = 0; v < tetromino_mesh_size; ++v) {
                graphics_block_ROM_vertex_t V = tetromino_ROM_mesh[v];
                vertex_buffer[v] = (graphics_block_vertex_t){.x=V.x, .y=V.y, .z=V.z,
                                                             .u=V.u, .v=V.v,
                                                             .block=t, .face=V.face};
            }
            glGenBuffers(1, &(vertex_buffer_ids[t][r]));
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[t][r]);
            glBufferData(/* type */  GL_ARRAY_BUFFER,
                         /* size */  tetromino_mesh_size*sizeof(graphics_block_vertex_t),
                         /* data */  vertex_buffer,
                         /* usage */ GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
/*}}}*/ }


void graphics_tetromino_end(void)
{ //{{{
    for (uint8_t t = 0; t < TETROMINO_TYPE_QUANTITY-1; ++t) {
        for (int r = 0; r < TETROMINO_ROTATION_QUANTITY; ++r) {
            glDeleteBuffers(/*buffer_quantity=*/1, &(vertex_buffer_ids[t][r]));
        }
    }
/*}}}*/ }


// static float model_matrix[16] = { [0]=1, [5]=1, [10]=1, [15]=1 };
typedef struct { int8_t x, y; } position_t;

static position_t falling_tetromino_position = {.x=0, .y=0};
void graphics_tetromino_position_falling_tetromino(uint8_t x, uint8_t y)
{
    falling_tetromino_position.x = x-3;
    falling_tetromino_position.y = PLAYFIELD_HEIGHT_1-y;
}

void graphics_tetromino_draw_falling_tetromino(const tetromino_t *t)
{ //{{{
    graphics_block_set_model_matrix((float[]){ [0]=1, [5]=1, [10]=1, [15]=1,
                                               [12]=falling_tetromino_position.x,
                                               [13]=falling_tetromino_position.y });
    graphics_block_draw(vertex_buffer_ids[t->type][t->rotation],
                        graphics_tetromino_get_mesh_size(t->type));
/*}}}*/ }


// void graphics_tetromino_active_orient(uint8_t x, uint8_t y)