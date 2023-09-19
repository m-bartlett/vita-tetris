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
void graphics_core_draw_HUD();
void graphics_draw_game();
void graphics_animate_game_over();


#endif