#include <stdint.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"
#include "tetromino_meshes.h"
#include "../engine/playfield.h"


// 6 vertices for each face of 5 visible faces for each of 4 blocks per tetromino
#define PREVIEW_SCALE 0.666
#define VERTEX_COUNT_MAX (6*5*4) 

// static GLuint vertex_buffer_id;
// static graphics_block_vertex_t vertex_buffer[VERTEX_COUNT_MAX];
static GLuint vertex_buffer_ids[TETROMINO_TYPE_QUANTITY][TETROMINO_ROTATION_QUANTITY];


void graphics_tetromino_init(void)
{ //{{{
    graphics_block_vertex_t vertex_buffer[TETROMINO_MEDIAN_MESH_SIZE];

    // Generate empty meshes for null tetromino type
    for (int r = 0; r < TETROMINO_ROTATION_QUANTITY; ++r) {
        glGenBuffers(1, &(vertex_buffer_ids[0][r]));
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[0][r]);
        glBufferData(/* type */  GL_ARRAY_BUFFER,
                     /* size */  0,
                     /* data */  vertex_buffer,
                     /* usage */ GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Read const ROM mesh vertices for actual tetromino species
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


// Controls for specific tetromino rendering

static inline void get_tetromino_center_offsets(const tetromino_type_t t,
                                                float *offset_x,
                                                float *offset_y)
{
    *offset_x = 0, *offset_y = 0;
    switch(t) {
        default: break;
        case TETROMINO_TYPE_I:
            *offset_y = 0.5; break;
        case TETROMINO_TYPE_S:
        case TETROMINO_TYPE_Z:
        case TETROMINO_TYPE_L:
        case TETROMINO_TYPE_J:
        case TETROMINO_TYPE_T:
            *offset_x=0.25; break;
    }
}


static struct { int8_t x, y; } falling_tetromino_position = {.x=0, .y=0};
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


static int8_t hard_drop_phantom_y = 0;
void graphics_tetromino_position_hard_drop_phantom(int8_t y)
{
    hard_drop_phantom_y = PLAYFIELD_HEIGHT_1-y;
}

void graphics_tetromino_draw_hard_drop_phantom(const tetromino_t *t)
{ //{{{
    graphics_block_set_model_matrix((const float[]){ [0]=1, [5]=1, [10]=1, [15]=1,
                                                     [12]=falling_tetromino_position.x,
                                                     [13]=hard_drop_phantom_y });
    glEnable(GL_BLEND); // The fragment shader returns fractional alpha, let's actually use it.
    graphics_block_draw(vertex_buffer_ids[t->type][t->rotation],
                        graphics_tetromino_get_mesh_size(t->type));
    glDisable(GL_BLEND);
/*}}}*/ }


void graphics_tetromino_draw_held_tetromino(const tetromino_type_t t)
{ //{{{
    float centering_offset_x, centering_offset_y;
    get_tetromino_center_offsets(t, &centering_offset_x, &centering_offset_y);
    graphics_block_set_model_matrix((const float[]){ [0]=PREVIEW_SCALE, [5]=PREVIEW_SCALE,
                                                     [10]=1, [15]=1,
                                                     [12]=-PLAYFIELD_WIDTH/2 + centering_offset_x,
                                                     [13]=PLAYFIELD_HEIGHT-6 - centering_offset_y});
    graphics_block_draw(vertex_buffer_ids[t][0], graphics_tetromino_get_mesh_size(t));
/*}}}*/ }


void graphics_tetromino_draw_queued_tetrominos(const tetromino_type_t* queue)
{ //{{{
    
    for (int i = 0; i < TETROMINO_QUEUE_PREVIEW_QUANTITY; ++i) {
        const tetromino_type_t t = queue[i]+1;

        float centering_offset_x, centering_offset_y;
        get_tetromino_center_offsets(t, &centering_offset_x, &centering_offset_y);

        graphics_block_set_model_matrix((const float[]){
            [0]=PREVIEW_SCALE, [5]=PREVIEW_SCALE,
            [10]=1, [15]=1,
            [12]=PLAYFIELD_WIDTH+2+centering_offset_x,
            [13]=(PLAYFIELD_HEIGHT*2/3 - centering_offset_y)-((i)*(3*PREVIEW_SCALE))
        });
        graphics_block_draw(vertex_buffer_ids[t][0], graphics_tetromino_get_mesh_size(t));
    }
/*}}}*/ }