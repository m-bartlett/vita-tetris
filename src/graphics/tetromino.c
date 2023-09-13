#include <stdint.h>
#include <stdbool.h>
#include "core.h"
#include "tetromino.h"
#include "../engine/playfield.h"


// 6 vertices for each face of 5 visible faces for each of 4 blocks per tetromino
#define PREVIEW_SCALE 0.666
#define TETROMINO_MAX_MESH_VERTICES 56


// static GLuint vertex_buffer_id;
static GLuint vertex_buffer_ids[TETROMINO_TYPE_QUANTITY][TETROMINO_ROTATION_QUANTITY];


void graphics_tetromino_init(void)
{ //{{{

    // NULL type tetromino inherently produces empty meshes since its grid is unpopulated
    graphics_block_vertex_t vertex_buffer[TETROMINO_MAX_MESH_VERTICES];
    uint16_t vertex_buffer_index;
    for (tetromino_type_t t = TETROMINO_TYPE_NULL; t < TETROMINO_TYPE_QUANTITY; ++t) {
        for (tetromino_rotation_t r = TETROMINO_ROTATION_0; r < TETROMINO_ROTATION_QUANTITY; ++r) {
            vertex_buffer_index = 0;
            const uint16_t grid = tetromino_get_grid_from_type_and_rotation(t, r);
            tetromino_grid_to_face_vertices(grid, t, vertex_buffer, &vertex_buffer_index);
            glGenBuffers(1, &(vertex_buffer_ids[t][r]));
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[t][r]);
            glBufferData(/* type */  GL_ARRAY_BUFFER,
                         /* size */  vertex_buffer_index*sizeof(graphics_block_vertex_t),
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



/*************************************************************************/
/*  Algorithm for generating minimal meshes for each tetromino rotation  */
/*************************************************************************/

    // TO-DO: Consider substituting u-v with corner enum

    #define add_block_vertex(x, y, z, u, v, block, face) \
        vertex_buffer[(*vertex_buffer_index)++] = (graphics_block_vertex_t){x,y,z,u,v,block,face};

    #define add_front_face(x, y, block) \
        add_block_vertex(x,   y,   1, 1, 1, block, FACE_FRONT) \
        add_block_vertex(x,   y+1, 1, 1, 2, block, FACE_FRONT) \
        add_block_vertex(x+1, y+1, 1, 2, 2, block, FACE_FRONT) \
        add_block_vertex(x+1, y,   1, 2, 1, block, FACE_FRONT)

    #define add_left_face(x, y, block) \
        add_block_vertex(x,  y,   0, 0, 1, block, FACE_LEFT) \
        add_block_vertex(x,  y+1, 0, 0, 2, block, FACE_LEFT) \
        add_block_vertex(x,  y+1, 1, 1, 2, block, FACE_LEFT) \
        add_block_vertex(x,  y,   1, 1, 1, block, FACE_LEFT)

    #define add_right_face(x, y, block) \
        add_block_vertex(x+1, y,   1, 2, 1, block, FACE_RIGHT) \
        add_block_vertex(x+1, y+1, 1, 2, 2, block, FACE_RIGHT) \
        add_block_vertex(x+1, y+1, 0, 3, 2, block, FACE_RIGHT) \
        add_block_vertex(x+1, y,   0, 3, 1, block, FACE_RIGHT)

    #define add_top_face(x, y, block) \
        add_block_vertex(x,   y+1, 1, 1, 2, block, FACE_TOP) \
        add_block_vertex(x,   y+1, 0, 1, 3, block, FACE_TOP) \
        add_block_vertex(x+1, y+1, 0, 2, 3, block, FACE_TOP) \
        add_block_vertex(x+1, y+1, 1, 2, 2, block, FACE_TOP)

    #define add_bottom_face(x, y, block) \
        add_block_vertex(x,   y,  0, 1, 0, block, FACE_BOTTOM) \
        add_block_vertex(x,   y,  1, 1, 1, block, FACE_BOTTOM) \
        add_block_vertex(x+1, y,  1, 2, 1, block, FACE_BOTTOM) \
        add_block_vertex(x+1, y,  0, 2, 0, block, FACE_BOTTOM)


    static inline uint16_t get_maskbit_for_grid_coordinate(uint8_t x, uint8_t y) {
        return 1<<(y*4+(3-x));
    }

    static inline void add_face_vertices_left_to_right(const uint16_t grid,
                                                       const tetromino_type_t block_type,
                                                       graphics_block_vertex_t *vertex_buffer,
                                                       uint16_t *vertex_buffer_index) {
        bool previous_grid_cell_occupied = false, current_grid_cell_occupied = false;
        uint16_t maskbit;
        for (uint8_t y = 0; y < 4; ++y) {
            previous_grid_cell_occupied = false;

            for (uint8_t x = 0; x < 4; ++x) {
                maskbit = get_maskbit_for_grid_coordinate(x,y);
                current_grid_cell_occupied = maskbit&grid;

                if (previous_grid_cell_occupied && current_grid_cell_occupied) {
                    add_front_face(x, y, block_type);
                }
                else if (!previous_grid_cell_occupied && current_grid_cell_occupied) {
                    add_left_face(x, y, block_type);
                    add_front_face(x, y, block_type);
                }
                else if (previous_grid_cell_occupied && (!current_grid_cell_occupied)) {
                    add_right_face(x-1, y, block_type);
                }

                previous_grid_cell_occupied = current_grid_cell_occupied;
            }

            if (previous_grid_cell_occupied) {
                add_right_face(3, y, block_type);
            }
        }
    }


    static inline void add_face_vertices_top_to_bottom(const uint16_t grid,
                                                       const tetromino_type_t block_type,
                                                       graphics_block_vertex_t *vertex_buffer,
                                                       uint16_t *vertex_buffer_index) {
        bool previous_grid_cell_occupied = false, current_grid_cell_occupied = false;
        uint16_t maskbit;
        for (uint8_t x = 0; x < 4; ++x) {
            previous_grid_cell_occupied = false;

            for (int8_t y = 3; y > -1; --y) {
                maskbit = get_maskbit_for_grid_coordinate(x,y);
                current_grid_cell_occupied = maskbit&grid;

                if (!previous_grid_cell_occupied && current_grid_cell_occupied) {
                    add_top_face(x, y, block_type);
                }
                else if (previous_grid_cell_occupied && (!current_grid_cell_occupied)) {
                    add_bottom_face(x, y+1, block_type);
                }

                previous_grid_cell_occupied = current_grid_cell_occupied;
            }

            if (previous_grid_cell_occupied) {
                add_bottom_face(x, 0, block_type);
            }
        }
    }


    void tetromino_grid_to_face_vertices(const uint16_t grid,
                                         const tetromino_type_t block_type,
                                         graphics_block_vertex_t *vertex_buffer,
                                         uint16_t *vertex_buffer_index) {
        add_face_vertices_left_to_right(grid, block_type, vertex_buffer, vertex_buffer_index);
        add_face_vertices_top_to_bottom(grid, block_type, vertex_buffer, vertex_buffer_index);
    }



/***********************************************/
/*  Controls for specific tetromino rendering  */
/***********************************************/


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
    graphics_block_draw(vertex_buffer_ids[t->type][t->rotation]);
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
    graphics_block_draw(vertex_buffer_ids[t->type][t->rotation]);
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
    graphics_block_draw(vertex_buffer_ids[t][0]);
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
        graphics_block_draw(vertex_buffer_ids[t][0]);
    }
/*}}}*/ }