#include "scoring.h"
#include "../graphics/text.h"


static uint8_t cleared_lines = 0;
static uint8_t total_cleared_lines = 0;
static uint8_t level = 0;
static uint32_t score = 0;

static const uint16_t SIMULTANEOUS_LINE_CLEAR_SCORES[] = { 100, 300, 500, 800 };


const uint8_t scoring_get_level() { return level; }
const uint32_t scoring_get_score() { return score; }
const uint16_t scoring_get_cleared_lines() { return total_cleared_lines; }


static inline void add_score(unsigned int x) {
    score += x;
    graphics_text_update_score_number(score);
}


void scoring_init() {
    cleared_lines = 0;
    total_cleared_lines = 0;
    level = 0;
    score = 0;
    graphics_text_update_score_number(score);
    graphics_text_update_cleared_lines_number(total_cleared_lines);
    graphics_text_update_level_number(level+1);
}


/* https://tetris.fandom.com/wiki/Scoring?so=search#Guideline_scoring_system */
const uint8_t scoring_add_line_clears(uint8_t lines)
{
    if (lines) {
        score += SIMULTANEOUS_LINE_CLEAR_SCORES[lines-1] * level;
        cleared_lines += lines;
        total_cleared_lines += lines;
        graphics_text_update_cleared_lines_number(total_cleared_lines);
        if (cleared_lines >= SCORING_LINES_PER_LEVEL) {
            cleared_lines %= SCORING_LINES_PER_LEVEL;
            if (level < SCORING_MAX_LEVEL ) {
                ++level;
                graphics_text_update_level_number(level+1);
                return level;
            }
            else return SCORING_MAX_LEVEL;
        }
    }
    return 0;
}


void scoring_add_soft_drop()
{
    add_score(SCORING_POINTS_PER_CELL_SOFT_DROP);
}


void scoring_add_hard_drop(uint8_t drop_height)
{
    add_score(SCORING_POINTS_PER_CELL_HARD_DROP * drop_height);
}