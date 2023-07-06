#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "tetromino.h"

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20
#define PLAYFIELD_SPAWN_Y 2
#define PLAYFIELD_SPAWN_X ((PLAYFIELD_WIDTH>>1)+1)


extern const uint8_t PLAYFIELD_WIDTH_1, PLAYFIELD_HEIGHT_1,
                     PLAYFIELD_WIDTH1,  PLAYFIELD_HEIGHT1;

typedef const int8_t (*playfield_view_t)[PLAYFIELD_WIDTH];
playfield_view_t playfield_view(void);

uint16_t playfield_get_4x4_vacancy_at_coordinate(uint8_t X, uint8_t Y);
void playfield_place_tetromino(tetromino_t* t, uint8_t X, uint8_t Y);
bool playfield_validate_tetromino_placement(tetromino_t* t, uint8_t X, uint8_t Y);
void playfield_clear_line(uint8_t Y);
uint8_t playfield_clear_lines(void (*callback)(uint8_t));
void playfield_set(const char* cells, const size_t size, const size_t offset);


#endif