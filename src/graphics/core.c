#include <stdio.h>
#include <string.h>
#include <vitasdk.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"
#include "playfield.h"
#include "background.h"
#include "border.h"
#include "text.h"

#include "../engine/core.h"
#include "../engine/playfield.h"
#include "../engine/scoring.h"
#include "../lib/shuffle.h"


void load_shader(const char *shader_path, GLuint *program) { //{{{
    GLuint shader_type;
    if (strstr(shader_path, ".vert") == NULL) shader_type = GL_FRAGMENT_SHADER;
    else {
      shader_type = GL_VERTEX_SHADER;
      printf("VERTEX SHADER: %s\n", shader_path);
    }
    FILE *f = fopen(shader_path, "r");
    if (!f) {printf("Shader error\n\n"); return; }
    fseek(f, 0, SEEK_END);
    const int32_t shader_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char shader_body[shader_size+1];
    fread(shader_body, 1, shader_size, f);
    fclose(f);
    shader_body[shader_size] = 0;
    printf("%s\n\n", shader_body);

    GLuint shader_ref = glCreateShader(shader_type);
    glShaderSource(/* shader */ shader_ref,
                   /* sources count */ 1,
                   /* array of string arrays */ (const char*[]){(const char*)&shader_body},
                   /* string length(s) */ &shader_size);
    glCompileShader(shader_ref);
    glAttachShader(*program, shader_ref);
/*}}}*/ }


static inline void vgl_init() {
    vglInitExtended(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x800000, SCE_GXM_MULTISAMPLE_4X);
    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    glFrontFace(GL_CW); 
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void graphics_init() {
    vgl_init();
    graphics_background_init();
    graphics_block_init();
    graphics_text_init();
    graphics_playfield_init();
    graphics_tetromino_init();
    graphics_border_init();
}

void graphics_end() {
    graphics_border_end();
    graphics_text_end();
    graphics_tetromino_end();
    graphics_playfield_end();
    graphics_block_end();
    graphics_background_end();
    vglEnd();
}


static inline void graphics_core_draw_queued_tetrominos()
{
    graphics_tetromino_draw_queued_tetrominos(engine_get_queued_tetrominos());
}


static inline void graphics_core_draw_held_tetromino()
{
    graphics_tetromino_draw_held_tetromino(engine_get_held_tetromino());
}


static inline void graphics_core_draw_falling_tetromino()
{
    graphics_tetromino_draw_falling_tetromino(engine_get_falling_tetromino());
}


static inline void graphics_core_draw_hard_drop_phantom()
{
    graphics_tetromino_draw_hard_drop_phantom(engine_get_falling_tetromino());
}


static inline void graphics_core_draw_score()
{
    // TO-DO implement this
    return;
}


void graphics_core_draw_HUD()
{  
    graphics_background_draw(); // instead of using glClear
    graphics_border_draw();
    graphics_text_draw();
    graphics_core_draw_held_tetromino();
    graphics_core_draw_queued_tetrominos();
    graphics_core_draw_score();
}


void graphics_core_draw_game()
{  
    graphics_core_draw_HUD();
    graphics_playfield_draw();
    graphics_core_draw_falling_tetromino();
    graphics_core_draw_hard_drop_phantom();
    vglSwapBuffers(GL_FALSE);
    return;
}


void graphics_core_animate_line_kill(uint8_t Y)
{
    uint8_t row_indices[PLAYFIELD_WIDTH];
    for (uint8_t x = 0; x < PLAYFIELD_WIDTH; ++x) row_indices[x]=x;
    shuffle(row_indices, sizeof(row_indices[0]), sizeof(row_indices));

    for (uint8_t x = 0; x < PLAYFIELD_WIDTH; ++x) {
        playfield_set_cell(0, row_indices[x], Y);
        graphics_playfield_update_mesh();
        graphics_core_draw_HUD();
        graphics_playfield_draw();
        vglSwapBuffers(GL_FALSE);
        sceKernelDelayThread(GRAPHICS_LINE_KILL_ANIMATION_FRAME_DELAY_MICROSECONDS);
    }
}


static void graphics_core_animate_playfield_fill(const uint8_t* new_playfield,
                                                 const size_t new_playfield_size)
{
    const uint16_t new_playfield_height = new_playfield_size / PLAYFIELD_WIDTH;
    const int16_t new_playfield_offset = abs(PLAYFIELD_HEIGHT-new_playfield_height)/2;

    uint8_t playfield_indices[new_playfield_size];
    for (uint8_t x = 0; x < new_playfield_size; ++x) playfield_indices[x]=x;
    shuffle(playfield_indices, sizeof(playfield_indices[0]), sizeof(playfield_indices));

    sceKernelDelayThread(500000);
    for (uint16_t c = 0; c < new_playfield_size; ++c) {
        uint16_t i = playfield_indices[c];
        uint8_t x = i % PLAYFIELD_WIDTH;
        uint8_t y = (i / PLAYFIELD_WIDTH) + new_playfield_offset;
        playfield_set_cell(new_playfield[i], x, y);
        if (c%4==0) {
            graphics_playfield_update_mesh();
            graphics_core_draw_HUD();
            graphics_playfield_draw();
            vglSwapBuffers(GL_FALSE);
        }
    }
    graphics_playfield_update_mesh();
    graphics_core_draw_HUD();
    graphics_playfield_draw();
    vglSwapBuffers(GL_FALSE);
    sceKernelDelayThread(1000000);
}


void graphics_core_animate_game_over()
{
    enum shorthand_tetrominos { I = TETROMINO_TYPE_I,
                                O = TETROMINO_TYPE_O,
                                T = TETROMINO_TYPE_T,
                                J = TETROMINO_TYPE_J,
                                L = TETROMINO_TYPE_L,
                                S = TETROMINO_TYPE_S,
                                Z = TETROMINO_TYPE_Z };

    const uint8_t game_over_playfield[] = {
        Z,Z,Z,0,0,0,0,0,0,0,
        Z,0,0,0,0,0,0,0,0,0,
        Z,0,Z,0,0,0,0,0,0,0,
        Z,Z,Z,I,0,0,0,0,0,0,
        0,0,I,0,I,0,0,0,0,0,
        0,0,I,I,I,0,0,0,0,0,
        0,0,I,0,I,L,0,L,0,0,
        0,0,0,0,L,0,L,0,L,0,
        T,T,T,0,L,0,L,0,J,J,
        T,0,T,0,0,0,0,0,J,J,
        T,0,T,0,0,0,0,0,J,0,
        T,T,S,0,S,0,0,0,J,J,
        0,0,S,0,S,0,0,0,0,0,
        0,0,S,0,S,J,J,0,0,0,
        0,0,0,S,0,J,J,0,0,0,
        0,0,0,0,0,J,0,O,O,O,
        0,0,0,0,0,J,J,O,0,O,
        0,0,0,0,0,0,0,O,O,0,
        0,0,0,0,0,0,0,O,0,O,
        0,0,0,0,0,0,0,0,0,0,
    };

    const size_t game_over_playfield_size = ARRAY_SIZE(game_over_playfield);

    graphics_core_animate_playfield_fill(game_over_playfield, game_over_playfield_size);
}


void graphics_core_animate_game_win()
{
    enum shorthand_tetrominos { I = TETROMINO_TYPE_I,
                                O = TETROMINO_TYPE_O,
                                T = TETROMINO_TYPE_T,
                                J = TETROMINO_TYPE_J,
                                L = TETROMINO_TYPE_L,
                                S = TETROMINO_TYPE_S,
                                Z = TETROMINO_TYPE_Z };

    const uint8_t game_win_playfield[] = {
        Z,0,Z,I,I,I,L,0,L,0,
        Z,0,Z,I,0,I,L,0,L,0,
        0,Z,0,I,0,I,L,0,L,0,
        0,Z,0,I,I,I,L,L,L,0,
        0,0,0,0,0,0,0,0,0,0,
        O,0,J,0,0,0,J,0,O,0,
        0,0,J,0,0,0,J,0,0,0,
        O,0,J,0,J,0,J,0,O,0,
        0,0,J,J,J,J,J,0,0,0,
        O,0,0,J,0,J,0,0,O,0,
        0,0,0,T,T,T,0,0,0,0,
        O,0,0,0,T,0,0,0,O,0,
        0,0,0,0,T,0,0,0,0,0,
        O,0,0,0,T,0,0,0,O,0,
        0,0,0,T,T,T,0,0,0,0,
        O,0,S,0,0,0,S,0,O,0,
        0,0,S,S,0,0,S,0,0,0,
        O,0,S,0,S,0,S,0,O,0,
        0,0,S,0,0,S,S,0,0,0,
        O,0,S,0,0,0,S,0,O,0,
    };

    const size_t game_win_playfield_size = ARRAY_SIZE(game_win_playfield);

    graphics_core_animate_playfield_fill(game_win_playfield, game_win_playfield_size);
}