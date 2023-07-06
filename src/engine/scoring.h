#ifndef SCORING_H
#define SCORING_H

#include <stdint.h>

#define SCORING_LINES_PER_LEVEL 10
#define SCORING_MAX_LEVEL 20
#define SCORING_POINTS_PER_CELL_SOFT_DROP 1
#define SCORING_POINTS_PER_CELL_HARD_DROP 2

const uint8_t scoring_get_level();
const uint32_t scoring_get_score();
const uint16_t scoring_get_cleared_lines();
const uint8_t scoring_add_line_clears(uint8_t lines);
void scoring_add_soft_drop();
void scoring_add_hard_drop(uint8_t drop_height);

#endif