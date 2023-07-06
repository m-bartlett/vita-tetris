#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdlib.h>
#include <vitaGL.h>
#include "../lib/linalg.h"
#include "../engine/tetromino.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544
#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#ifdef USE_SINGLE_TEXTURE_OBJ
#endif

void load_shader(const char *shader_path, GLuint *program);
void graphics_init();
void graphics_end();
void graphics_draw_tetromino_at_xy(const tetromino_t *t, const uint8_t X, const uint8_t Y);
void graphics_draw_playfield();
void graphics_draw_queue_preview();
void graphics_draw_score();
void graphics_draw_held_tetromino();
void graphics_draw_screen();
void graphics_draw_active_tetromino();
void graphics_draw_hard_drop_preview();
void graphics_draw_game();
void graphics_animate_line_kill(uint8_t Y);
void graphics_animate_game_over();


#endif