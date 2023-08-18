#include <stdio.h>
#include <string.h>
#include "core.h"
#include "block.h"
#include "tetromino.h"
#include "playfield.h"
#include "background.h"
#include "text.h"

#include "../engine/core.h"
#include "../engine/scoring.h"

#define FRAME_DELAY_us 15000

#define TETROMINO_QUEUE_PREVIEW_QUANTITY 6
#define TETROMINO_QUEUE_PREVIEW_HEIGHT (TETROMINO_QUEUE_PREVIEW_QUANTITY*3)

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
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CW); 
   glClearColor(0.1, 0.1, 0.1, 1.0);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void graphics_init() {
   vgl_init();
   graphics_background_init();
   graphics_block_init();
   // graphics_text_init();
   // graphics_menu_init();
   graphics_playfield_init();
   graphics_tetromino_init();
}

void graphics_end() {
   graphics_tetromino_end();
   graphics_playfield_end();
   graphics_block_end();
   graphics_background_end();
   vglEnd();
}


void graphics_draw_queue_preview()
{
   return;
}


void graphics_draw_score()
{
   return;
}


void graphics_draw_held_tetromino()
{
   return;
}


void graphics_draw_active_tetromino()
{
   graphics_tetromino_draw(engine_get_active_tetromino());
   return;
}


void graphics_draw_hard_drop_preview()
{
   engine_update_hard_drop_y();
   return;
}


void graphics_draw_game()
{  
   graphics_background_draw(); // instead of using glClear

   /* graphics_block_set_model_matrix */
   graphics_playfield_draw();

   graphics_draw_active_tetromino();

   // enable alpha, set blend    
   graphics_draw_hard_drop_preview();

   graphics_draw_held_tetromino();

   /*   queue preview
    - queue_preview_to_vertices() every piece update
    - disable alpha if needed
    - set translation uniforms
    - draw_queue_preview*/

   graphics_draw_held_tetromino();

   graphics_draw_score();

   vglSwapBuffers(GL_FALSE);
   return;
}


void graphics_animate_line_kill(uint8_t Y)
{
   // TO-DO animate the line kill
   graphics_playfield_update_mesh();
   return;
}


void graphics_animate_game_over()
{
   return;
}