#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "tetromino.h"

#define ENGINE_DROP_LOCK_DELAY_MICROSECONDS 500000
#define ENGINE_GRAVITY_INITIAL_DELAY_MICROSECONDS 700000
#define ENGINE_FRAMES_PER_SECOND 30
#define ENGINE_MICROSECONDS_PER_FRAME (1000000/ENGINE_FRAMES_PER_SECOND)


enum engine_state_enum { ENGINE_STATE_NULL=0,
                         ENGINE_STATE_RUNNING,
                         ENGINE_STATE_LOSE,
                         ENGINE_STATE_WIN,
                         ENGINE_STATE_QUANTITY };

typedef enum engine_state_enum engine_state_t;
typedef struct { const uint8_t x; const uint8_t y; } point_t;


const tetromino_t* engine_get_falling_tetromino();
const tetromino_type_t engine_get_held_tetromino();
const tetromino_type_t* engine_get_queued_tetrominos();
const point_t engine_get_active_xy();
const int8_t engine_update_hard_drop_y();

void engine_init();
void engine_end();
void engine_game_loop(void);
void engine_spawn_tetromino(tetromino_type_t type);
tetromino_type_t engine_pop_queued_tetromino();
bool engine_move_falling_tetromino(int8_t dx, uint8_t dy);
void engine_swap_held_tetromino_with_active(void);
void engine_place_tetromino_at_xy(uint8_t x, uint8_t y);
void engine_rotate_falling_tetromino_clockwise();
void engine_rotate_falling_tetromino_counterclockwise();
void engine_hard_drop_tetromino();
void engine_soft_drop_tetromino();
uint32_t engine_rng_get_sample();

#endif