#include <stdio.h>
#include <string.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"
#include "playfield.h"
#include "background.h"
#include "border.h"
#include "text.h"

#include "../engine/core.h"
#include "../engine/scoring.h"

/*
static const uint8_t GAME_OVER_PLAYFIELD[] = {
    TETROMINO_TYPE_Z,TETROMINO_TYPE_Z,TETROMINO_TYPE_Z,0,0,0,0,0,0,0,
    TETROMINO_TYPE_Z,0,0,0,0,0,0,0,0,0,
    TETROMINO_TYPE_Z,0,TETROMINO_TYPE_Z,0,0,0,0,0,0,0,
    TETROMINO_TYPE_Z,TETROMINO_TYPE_Z,TETROMINO_TYPE_Z,TETROMINO_TYPE_I,0,0,0,0,0,0,
    0,0,TETROMINO_TYPE_I,0,TETROMINO_TYPE_I,0,0,0,0,0,
    0,0,TETROMINO_TYPE_I,TETROMINO_TYPE_I,TETROMINO_TYPE_I,0,0,0,0,0,
    0,0,TETROMINO_TYPE_I,0,TETROMINO_TYPE_I,TETROMINO_TYPE_L,0,TETROMINO_TYPE_L,0,0,
    0,0,0,0,TETROMINO_TYPE_L,0,TETROMINO_TYPE_L,0,TETROMINO_TYPE_L,0,
    TETROMINO_TYPE_T,TETROMINO_TYPE_T,TETROMINO_TYPE_T,0,TETROMINO_TYPE_L,
            0,TETROMINO_TYPE_L,0,TETROMINO_TYPE_J,TETROMINO_TYPE_J,
    TETROMINO_TYPE_T,0,TETROMINO_TYPE_T,0,0,0,0,0,TETROMINO_TYPE_J,TETROMINO_TYPE_J,
    TETROMINO_TYPE_T,0,TETROMINO_TYPE_T,0,0,0,0,0,TETROMINO_TYPE_J,0,
    TETROMINO_TYPE_T,TETROMINO_TYPE_T,TETROMINO_TYPE_S,0,TETROMINO_TYPE_S,
            0,0,0,TETROMINO_TYPE_J,TETROMINO_TYPE_J,
    0,0,TETROMINO_TYPE_S,0,TETROMINO_TYPE_S,0,0,0,0,0,
    0,0,TETROMINO_TYPE_S,0,TETROMINO_TYPE_S,TETROMINO_TYPE_J,TETROMINO_TYPE_J,0,0,0,
    0,0,0,TETROMINO_TYPE_S,0,TETROMINO_TYPE_J,TETROMINO_TYPE_J,0,0,0,
    0,0,0,0,0,TETROMINO_TYPE_J,0,TETROMINO_TYPE_O,TETROMINO_TYPE_O,TETROMINO_TYPE_O,
    0,0,0,0,0,TETROMINO_TYPE_J,TETROMINO_TYPE_J,TETROMINO_TYPE_O,0,TETROMINO_TYPE_O,
    0,0,0,0,0,0,0,TETROMINO_TYPE_O,TETROMINO_TYPE_O,0,
    0,0,0,0,0,0,0,TETROMINO_TYPE_O,0,TETROMINO_TYPE_O
};

static const size_t GAME_OVER_PLAYFIELD_SIZE = ARRAY_SIZE(GAME_OVER_PLAYFIELD);
*/


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


void graphics_draw_game()
{  
   graphics_core_draw_HUD();
   graphics_playfield_draw();
   graphics_core_draw_falling_tetromino();
   graphics_core_draw_hard_drop_phantom();
   vglSwapBuffers(GL_FALSE);
   return;
}


void graphics_animate_game_over()
{
   return;
}