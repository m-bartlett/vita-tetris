#ifndef GRAPHICS_TEXT_H
#define GRAPHICS_TEXT_H

void graphics_text_init(void); 
void graphics_text_end(void);
void graphics_text_draw(void);
void graphics_text_update_score_number(unsigned int score);
void graphics_text_update_cleared_lines_number(unsigned short lines);
void graphics_text_update_level_number(unsigned char level);

#endif