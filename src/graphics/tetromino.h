/********************************************************************************
 **                                                                            **
 **    Pre-define meshes for each tetromino type and expose drawing methods    **
 **                                                                            **
 ********************************************************************************/

#ifndef GRAPHICS_TETROMINO_H
#define GRAPHICS_TETROMINO_H 

#include "../engine/tetromino.h"


void graphics_tetromino_init(void);
void graphics_tetromino_end(void);
void graphics_tetromino_position_falling_tetromino(uint8_t x, uint8_t y);
void graphics_tetromino_draw_falling_tetromino(const tetromino_t *t);
void graphics_tetromino_position_hard_drop_phantom(int8_t y);
void graphics_tetromino_draw_hard_drop_phantom(const tetromino_t *t);
void graphics_tetromino_draw_held_tetromino(const tetromino_type_t t);
void graphics_tetromino_draw_queued_tetrominos(const tetromino_type_t* queue);

#endif