#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <stdint.h>
#include "tetromino.h"

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20

extern const uint8_t PLAYFIELD_WIDTH_1, PLAYFIELD_HEIGHT_1;

uint16_t playfield_get_4x4_vacancy_at_coordinate(uint8_t X, uint8_t Y);
void playfield_place_tetromino(tetromino_t* t, uint8_t X, uint8_t Y);
void playfield_print(void);

const int8_t** playfield_view(void);

#endif