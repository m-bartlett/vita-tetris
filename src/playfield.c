#include <stdio.h>
#include <stdbool.h>
#include "playfield.h"

const uint8_t PLAYFIELD_WIDTH_1 = PLAYFIELD_WIDTH-1, PLAYFIELD_HEIGHT_1 = PLAYFIELD_HEIGHT-1;
static const uint8_t X_MAX = PLAYFIELD_WIDTH+2;
static int8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]={{0}};

uint16_t playfield_get_4x4_vacancy_at_coordinate(uint8_t X, uint8_t Y) {/*{{{*/
    const uint8_t X1=X+1, Y1=Y+1;
    uint16_t grid = (uint16_t)0x0000;
    uint8_t mask_bit = 15;
    
    for (int8_t y=Y-3; y < Y1; ++y) {
        if (y < 0 || y > PLAYFIELD_HEIGHT_1) {  // rows above and below the playfield are occupied
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

bool playfield_validate_tetromino_placement(tetromino_t* t, uint8_t X, uint8_t Y) {
    uint16_t tetromino_grid = tetromino_get_grid(t);
    uint16_t placement_grid = playfield_get_4x4_vacancy_at_coordinate(X,Y);
    return tetromino_grid & placement_grid == 0;
}

void playfield_place_tetromino(tetromino_t* t, uint8_t X, uint8_t Y) {
    const enum tetromino_type_t symbol = t->type;
    const uint8_t X1=X+1, Y1=Y+1;
    const uint16_t grid = tetromino_get_grid(t);
    uint16_t maskbit = (uint16_t)1<<15;
    
    for (int8_t y=Y-3; y < Y1; ++y) {
        if (y < 0 || y > PLAYFIELD_HEIGHT_1) {  // can't modify outside payfield bounds
            maskbit>>=4;
            continue;
        }
        for (int8_t x=X-3; x < X1; ++x) {
            if (x < 0 || x > PLAYFIELD_WIDTH_1) { /* left and right of playfield are occupied */
                continue;
            }
            if (maskbit&grid) PLAYFIELD[y][x]=(int8_t)symbol;
            maskbit >>=1 ;
        }
    }
}

void playfield_print(void) {
    printf(" ");
    for (int x = 0; x < PLAYFIELD_WIDTH_1; ++x) printf("_");
    printf("\n");
    for (int y = 0; y < PLAYFIELD_HEIGHT_1; ++y) {
        printf("|");
        for (int x = 0; x < PLAYFIELD_WIDTH_1; ++x) {
            char c = tetromino_type_t2char((enum tetromino_type_t)PLAYFIELD[y][x]);
            if (c == '\0') c = ' ';
            printf("%c", c);
        }
        printf("|\n");
    }
    printf(" ");
    for (int x = 0; x < PLAYFIELD_WIDTH_1; ++x) printf("^");
    printf("\n");
}


const (const int8_t*)* playfield_view(void) {
    return (const int8_t**)PLAYFIELD;
}