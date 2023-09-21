#ifndef GRAPHICS_PLAYFIELD_H
#define GRAPHICS_PLAYFIELD_H

void graphics_playfield_init(void);
void graphics_playfield_end(void);
void graphics_playfield_update_mesh(void);
void graphics_playfield_draw(void);
void graphics_playfield_set_model_matrix(float buffer[16]);

#endif