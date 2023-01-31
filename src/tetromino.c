#include "tetromino.h"
#include <stdlib.h>

// static const enum tetromino_type_t
// INDEX2TETROMINO_TYPE_T[TETROMINO_TYPE_QUANTITY-1] = {I, O, T, J, L, S, Z};

static const char
TETROMINO_TYPE_T2CHAR[TETROMINO_TYPE_QUANTITY] = {
  [TETROMINO_TYPE_NULL] = '\0',
  [I]='I', [O]='O', [T]='T', [J]='J', [L]='L', [S]='S', [Z]='Z'
};

static const uint16_t
TETROMINO_ROTATIONS[TETROMINO_TYPE_QUANTITY][4] = {
  [TETROMINO_TYPE_NULL] = {0,0,0,0},
  [I] = BITGRID_4x4x4_TO_UINT16x4(0000, 0010, 0000, 0100,
                                  1111, 0010, 0000, 0100,
                                  0000, 0010, 1111, 0100,
                                  0000, 0010, 0000, 0100),

  [O] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  0110, 0110, 0110, 0110,
                                  0110, 0110, 0110, 0110,
                                  0000, 0000, 0000, 0000),

  [T] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  0100, 0100, 0000, 0100,
                                  1110, 0110, 1110, 1100,
                                  0000, 0100, 0100, 0100),

  [J] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  1000, 0110, 0000, 0100,
                                  1110, 0100, 1110, 0100,
                                  0000, 0100, 0010, 1100),

  [L] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  0010, 0100, 0000, 1100,
                                  1110, 0100, 1110, 0100,
                                  0000, 0110, 1000, 0100),

  [S] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  0110, 0100, 0000, 1000,
                                  1100, 0110, 0110, 1100,
                                  0000, 0010, 1100, 0100),

  [Z] = BITGRID_4x4x4_TO_UINT16x4(0000, 0000, 0000, 0000,
                                  1100, 0010, 0000, 0100,
                                  0110, 0110, 1100, 1100,
                                  0000, 0100, 0110, 1000)
};

// static uint8_t tetromino_bag_of_7_index = 0;
// static enum tetromino_type_t tetromino_bag_of_7_order[] = {L, I, O, T, S, Z, J};
// /* TODO: 2 separate index views to have double bags (show future pieces despite randomness) */
// enum tetromino_type_t tetromino_bag_of_7_shuffle(void) {
//     // srand((unsigned) time(&t));
//     for (int i = 0; i < 7; ++i) {
//         uint8_t source = rand() & 7; // x%7 == x&7
//         uint8_t target= rand() & 7;
//         enum tetromino_type_t t;
//         t = tetromino_bag_of_7_order[target];
//         tetromino_bag_of_7_order[target] = tetromino_bag_of_7_order[source];
//         tetromino_bag_of_7_order[source] = t;
//     }
// }

const char tetromino_type_t2char(enum tetromino_type_t t) { 
    return TETROMINO_TYPE_T2CHAR[t];
}

const char tetromino_get_type_char(tetromino_t *t) { 
    return tetromino_type_t2char(t->type);
}

const uint16_t tetromino_get_grid(tetromino_t *t) {
    return TETROMINO_ROTATIONS[t->type][t->rotation];
}

const uint16_t tetromino_rotate_clockwise(tetromino_t *t) {
    t->rotation = (t->rotation+1) & 0b11;  // n&3 == n%3
    return tetromino_get_grid(t);
}

const uint16_t tetromino_rotate_counterclockwise(tetromino_t *t) {
    t->rotation = (t->rotation-1) & 0b11;  // n&3 == n%3
    return tetromino_get_grid(t);
}