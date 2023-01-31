#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdint.h>

// Notice that this tranposes the bit-quads
#define BITGRID_4x4x4_TO_UINT16x4(a1,b1,c1,d1,\
                                  a2,b2,c2,d2,\
                                  a3,b3,c3,d3,\
                                  a4,b4,c4,d4) { ((uint16_t)0b##a1##a2##a3##a4),\
                                                 ((uint16_t)0b##b1##b2##b3##b4),\
                                                 ((uint16_t)0b##c1##c2##c3##c4),\
                                                 ((uint16_t)0b##d1##d2##d3##d4) }


enum tetromino_type_t { TETROMINO_TYPE_NULL=0,
                        I=1, O=2, T=3, J=4, L=5, S=6, Z=7,
                        TETROMINO_TYPE_QUANTITY};

typedef struct { enum tetromino_type_t type; uint8_t rotation; } tetromino_t;

const char tetromino_type_t2char(enum tetromino_type_t t);
const char tetromino_get_type_char(tetromino_t *t);
uint16_t tetromino_get_grid(tetromino_t *t);
uint16_t tetromino_rotate_clockwise(tetromino_t *t);
uint16_t tetromino_rotate_counterclockwise(tetromino_t *t);

#endif