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
void graphics_tetromino_to_block_vertices(const tetromino_t *t);
void graphics_tetromino_draw_buffer();
void graphics_tetromino_draw(const tetromino_t *t);


#endif