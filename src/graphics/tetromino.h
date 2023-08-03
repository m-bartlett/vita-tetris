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
void graphics_tetromino_draw(const tetromino_t *t);
void graphics_tetromino_set_model_orientation(const tetromino_t *t, uint8_t x, uint8_t y);

#endif