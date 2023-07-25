#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#define _GNU_SOURCE

#include "engine/core.h"
#include "graphics/core.h"

int main(int argc, char *argv[]) {
    engine_init();
    graphics_init();

    engine_game_loop();

    graphics_end();
    engine_end();
    return 0;
}