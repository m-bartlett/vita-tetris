#include "scoring.h"

static uint8_t cleared_lines = 0;
static uint8_t total_cleared_lines = 0;
static uint8_t level = 0;
static uint32_t score = 0;

static const uint16_t SIMULTANEOUS_LINE_CLEAR_SCORES[] = { 100, 300, 500, 800 };


const uint8_t scoring_get_level() { return level; }
const uint32_t scoring_get_score() { return score; }
const uint16_t scoring_get_cleared_lines() { return total_cleared_lines; }



/* https://tetris.fandom.com/wiki/Scoring?so=search#Guideline_scoring_system */
const uint8_t scoring_add_line_clears(uint8_t lines)
{
    if (lines) {
        score += SIMULTANEOUS_LINE_CLEAR_SCORES[lines-1] * level;
        cleared_lines += lines;
        total_cleared_lines += lines;
        if (cleared_lines >= SCORING_LINES_PER_LEVEL) {
            cleared_lines %= SCORING_LINES_PER_LEVEL;
            if (level < SCORING_MAX_LEVEL ) return ++level;
        }
    }
    return 0;
}


void scoring_add_soft_drop()
{
    score += SCORING_POINTS_PER_CELL_SOFT_DROP;
}


void scoring_add_hard_drop(uint8_t drop_height)
{
    score += SCORING_POINTS_PER_CELL_HARD_DROP * drop_height;
}