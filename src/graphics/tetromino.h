/********************************************************************************
 **                                                                            **
 **    Pre-define meshes for each tetromino type and expose drawing methods    **
 **                                                                            **
 ********************************************************************************/

#ifndef GRAPHICS_TETROMINO_H
#define GRAPHICS_TETROMINO_H 

#include "block.h"
#include "../engine/tetromino.h"


void graphics_tetromino_init(void);
void graphics_tetromino_end(void);
void tetromino_grid_to_face_vertices(const uint16_t grid,
                                     const tetromino_type_t block_type,
                                     graphics_block_vertex_t *vertex_buffer,
                                     uint16_t *vertex_buffer_index);
void graphics_tetromino_position_falling_tetromino(uint8_t x, uint8_t y);
void graphics_tetromino_draw_falling_tetromino(const tetromino_t *t);
void graphics_tetromino_position_hard_drop_phantom(int8_t y);
void graphics_tetromino_draw_hard_drop_phantom(const tetromino_t *t);
void graphics_tetromino_draw_held_tetromino(const tetromino_type_t t);
void graphics_tetromino_draw_queued_tetrominos(const tetromino_type_t* queue);

#endif