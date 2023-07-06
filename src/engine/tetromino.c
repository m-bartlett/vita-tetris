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

static const char
TETROMINO_TYPE_T2CHAR[TETROMINO_TYPE_QUANTITY] = { [TETROMINO_TYPE_NULL] = '\0',
                                                   [TETROMINO_TYPE_I]    = 'I',
                                                   [TETROMINO_TYPE_O]    = 'O',
                                                   [TETROMINO_TYPE_T]    = 'T',
                                                   [TETROMINO_TYPE_J]    = 'J',
                                                   [TETROMINO_TYPE_L]    = 'L',
                                                   [TETROMINO_TYPE_S]    = 'S',
                                                   [TETROMINO_TYPE_Z]    = 'Z' };

static const uint16_t
TETROMINO_ROTATIONS[TETROMINO_TYPE_QUANTITY][4] = {
  
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


const char tetromino_type_t2char(const tetromino_type_t t)
{
  return TETROMINO_TYPE_T2CHAR[t];
}


const tetromino_type_t index2tetromino_type_t(const uint8_t i)
{
  return INDEX2TETROMINO_TYPE_T[0];
}


const char tetromino_get_type_char(const tetromino_t *t)
{
  return tetromino_type_t2char(t->type);
}


const uint16_t tetromino_get_grid(const tetromino_t *t)
{
  return TETROMINO_ROTATIONS[t->type][t->rotation];
}


void tetromino_rotate_clockwise(tetromino_t *t)
{
    t->rotation = (t->rotation+1) & 0b11;  // n&3 == n%3
}


void tetromino_rotate_counterclockwise(tetromino_t *t)
{
    t->rotation = (t->rotation-1) & 0b11;  // n&3 == n%3
}