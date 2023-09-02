/********************************************************************************
 **                                                                            **
 **  GL resource initialization and abstraction of drawing border lines around **
 **  different regions of the game UI to help compartmentalize them visually.  **
 **                                                                            **
 ********************************************************************************/

#ifndef GRAPHICS_BORDER_H
#define GRAPHICS_BORDER_H

#include "core.h"

void graphics_border_init(void);
void graphics_border_end(void);
void graphics_border_draw(void);

#endif