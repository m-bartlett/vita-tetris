#include <stddef.h>  // NULL
#include <string.h>  // strlen
#include "playfield.h"

const uint8_t PLAYFIELD_WIDTH_1 = PLAYFIELD_WIDTH - 1, PLAYFIELD_HEIGHT_1 = PLAYFIELD_HEIGHT - 1,
              PLAYFIELD_WIDTH1  = PLAYFIELD_WIDTH + 1, PLAYFIELD_HEIGHT1  = PLAYFIELD_HEIGHT + 1;

static const uint8_t X_MAX = PLAYFIELD_WIDTH+2;
static int8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]={{0}};


playfield_view_t playfield_view(void) { return (playfield_view_t)PLAYFIELD; }


uint16_t playfield_get_4x4_vacancy_at_coordinate(uint8_t X, uint8_t Y)
{/*{{{*/
    const uint8_t X1=X+1, Y1=Y+1;
    uint16_t grid = (uint16_t)0x0000;
    uint8_t mask_bit = 15;
    
    for (int8_t y=Y-3; y < Y1; ++y) {
        if (y<0) {
            mask_bit -= 4;
            continue;
        }
        if (y > PLAYFIELD_HEIGHT_1) {  // rows above and below the playfield are occupied
            mask_bit -= 3;
            grid |= 0b1111 << mask_bit;
            mask_bit--;
            continue;
        }
        for (int8_t x=X-3; x < X1; ++x) {
            if (x < 0 || x > PLAYFIELD_WIDTH_1 /* left and right of playfield are occupied */ 
                || PLAYFIELD[y][x] > 0) {      /* nonzero indices are occupied */
                grid |= 1 << mask_bit;
            }
            mask_bit--;
        }
    }
    return grid;
/*}}}*/}


bool playfield_validate_tetromino_placement(tetromino_t* t, uint8_t X, uint8_t Y)
{ //{{{
    uint16_t tetromino_grid = tetromino_get_grid(t);
    uint16_t placement_grid = playfield_get_4x4_vacancy_at_coordinate(X,Y);
    return (tetromino_grid & placement_grid) == 0;
/*}}}*/ }


void playfield_place_tetromino(tetromino_t* t, uint8_t X, uint8_t Y)
{ //{{{
    const tetromino_type_t block_type = t->type;
    const uint8_t X1=X+1, Y1=Y+1;
    const uint16_t grid = tetromino_get_grid(t);
    uint16_t maskbit = (uint16_t)1<<15;
    
    for (int8_t y=Y-3; y < Y1; ++y) {
        if (y < 0 || y > PLAYFIELD_HEIGHT) {  // can't modify outside payfield bounds
            maskbit>>=4;
            continue;
        }
        for (int8_t x=X-3; x < X1; ++x) {
            if ( !(x < 0 || x > PLAYFIELD_WIDTH) && (maskbit&grid) ) {
                PLAYFIELD[y][x] = (int8_t)block_type;
            }
            maskbit >>=1 ;
        }
    }
/*}}}*/ }


void playfield_clear_line(uint8_t Y)
{ //{{{
    int8_t y = Y-1;
    uint8_t y1 = Y;
    while (y > -1) { // copy all above rows each down one at a time
        for (uint8_t x = 0; x < PLAYFIELD_WIDTH; ++x) {
            PLAYFIELD[y1][x] = PLAYFIELD[y][x];
        }
        --y;
        --y1;
    }
    for (uint8_t x = 0; x < PLAYFIELD_WIDTH; ++x) PLAYFIELD[0][x] = 0;
/*}}}*/ }


uint8_t playfield_clear_lines(void (*callback)(uint8_t))
{ //{{{
    uint8_t lines = 0;
    for (uint8_t y = 0; y < PLAYFIELD_HEIGHT; ++y) {
        bool row_occupied = true;
        for (uint8_t x = 0; x < PLAYFIELD_WIDTH; ++x) {
            row_occupied = row_occupied && PLAYFIELD[y][x];
        }
        if (row_occupied) {
            playfield_clear_line(y);
            if (callback != NULL) callback(y);
            ++lines;
        }
    }
    return lines;
/*}}}*/ }

void playfield_set(const char* cells, const size_t size, const size_t offset)
{ //{{{
    if (offset+size > PLAYFIELD_HEIGHT*PLAYFIELD_WIDTH) return;
    for (uint16_t i_p = offset, i_c=0; i_c < size; ++i_c, ++i_p) {
        uint8_t x = i_p % PLAYFIELD_WIDTH, y = i_p / PLAYFIELD_WIDTH;
        PLAYFIELD[y][x] = cells[i_c];
    }
/*}}}*/ }