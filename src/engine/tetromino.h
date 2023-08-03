#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdint.h>

enum tetromino_type_enum { TETROMINO_TYPE_NULL = 0,
                           TETROMINO_TYPE_I    = 1,
                           TETROMINO_TYPE_O    = 2,
                           TETROMINO_TYPE_T    = 3,
                           TETROMINO_TYPE_J    = 4,
                           TETROMINO_TYPE_L    = 5,
                           TETROMINO_TYPE_S    = 6,
                           TETROMINO_TYPE_Z    = 7,
                           TETROMINO_TYPE_QUANTITY };
typedef enum tetromino_type_enum tetromino_type_t;

enum tetromino_rotation_enum { TETROMINO_ROTATION_0   = 0,
                               TETROMINO_ROTATION_90  = 1,
                               TETROMINO_ROTATION_180 = 2,
                               TETROMINO_ROTATION_270 = 3,
                               TETROMINO_ROTATION_QUANTITY };
typedef enum tetromino_rotation_enum tetromino_rotation_t;

typedef struct { tetromino_type_t type; uint8_t rotation; } tetromino_t;


// Notice that this tranposes the bit-quads
#define BITGRID_4x4x4_TO_UINT16x4(a1,b1,c1,d1,\
                                  a2,b2,c2,d2,\
                                  a3,b3,c3,d3,\
                                  a4,b4,c4,d4)\
{\
  [TETROMINO_ROTATION_0]   = ((uint16_t)0b##a1##a2##a3##a4),\
  [TETROMINO_ROTATION_90]  = ((uint16_t)0b##b1##b2##b3##b4),\
  [TETROMINO_ROTATION_180] = ((uint16_t)0b##c1##c2##c3##c4),\
  [TETROMINO_ROTATION_270] = ((uint16_t)0b##d1##d2##d3##d4)\
}

const uint16_t tetromino_get_grid(const tetromino_t *t);
void tetromino_rotate_clockwise(tetromino_t *t);
void tetromino_rotate_counterclockwise(tetromino_t *t);

#endif
