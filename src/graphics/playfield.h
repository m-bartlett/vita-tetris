#ifndef GRAPHICS_PLAYFIELD_H
#define GRAPHICS_PLAYFIELD_H

#define PLAYFIELD_LINE_KILL_ANIMATION_FRAME_DELAY_MICROSECONDS 30000

void graphics_playfield_init(void);
void graphics_playfield_end(void);
void graphics_playfield_update_mesh(void);
void graphics_playfield_draw(void);
void graphics_playfield_animate_line_kill(uint8_t Y);

#endif