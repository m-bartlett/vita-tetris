#include "tetromino.h"
#include <stdlib.h>

static const tetromino_type_t
INDEX2TETROMINO_TYPE_T[TETROMINO_TYPE_QUANTITY-1] = { TETROMINO_TYPE_I,
                                                      TETROMINO_TYPE_O,
                                                      TETROMINO_TYPE_T,
                                                      TETROMINO_TYPE_J,
                                                      TETROMINO_TYPE_L,
                                                      TETROMINO_TYPE_S,
                                                      TETROMINO_TYPE_Z };

static const uint16_t
TETROMINO_ROTATIONS[TETROMINO_TYPE_QUANTITY][TETROMINO_ROTATION_QUANTITY] = {
  
  [TETROMINO_TYPE_NULL] = {0,0,0,0},
                                    // rotation =  0    90   180   270 
  [TETROMINO_TYPE_I] = BITGRID_4x4x4_TO_UINT16x4(0000, 0010, 0000, 0100,
                                                 1111, 0010, 0000, 0100,
                                                 0000, 0010, 1111, 0100,
                                                 0000, 0010, 0000, 0100),

  [TETROMINO_TYPE_O] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 0110, 0110, 0110, 0110,
                                                 0110, 0110, 0110, 0110,
                                                 0000, 0000, 0000, 0000),

  [TETROMINO_TYPE_T] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 0100, 0100, 0000, 0100,
                                                 1110, 0110, 1110, 1100,
                                                 0000, 0100, 0100, 0100),

  [TETROMINO_TYPE_J] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 1000, 0110, 0000, 0100,
                                                 1110, 0100, 1110, 0100,
                                                 0000, 0100, 0010, 1100),

  [TETROMINO_TYPE_L] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 0010, 0100, 0000, 1100,
                                                 1110, 0100, 1110, 0100,
                                                 0000, 0110, 1000, 0100),

  [TETROMINO_TYPE_S] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 0110, 0100, 0000, 1000,
                                                 1100, 0110, 0110, 1100,
                                                 0000, 0010, 1100, 0100),

  [TETROMINO_TYPE_Z] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                                 1100, 0010, 0000, 0100,
                                                 0110, 0110, 1100, 1100,
                                                 0000, 0100, 0110, 1000)
};


const tetromino_type_t index2tetromino_type_t(const uint8_t i)
{
  return INDEX2TETROMINO_TYPE_T[0];
}


const uint16_t tetromino_get_grid(const tetromino_t *t)
{
  return TETROMINO_ROTATIONS[t->type][t->rotation];
}


const uint16_t tetromino_get_grid_from_type_and_rotation(const tetromino_type_t t,
                                                         const tetromino_rotation_t r)
{
  return TETROMINO_ROTATIONS[t][r];
}


void tetromino_rotate_clockwise(tetromino_t *t)
{
    t->rotation = (t->rotation+1) & 0b11;  // n&3 == n%3
}


void tetromino_rotate_counterclockwise(tetromino_t *t)
{
    t->rotation = (t->rotation-1) & 0b11;  // n&3 == n%3
}
