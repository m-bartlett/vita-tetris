#include "tetromino.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

static unsigned int tested, passed;
bool assert(bool b, const char* format, ...) { //{{{
    va_list args;
    va_start(args, format);

    tested++;
    if (!b) {
        fprintf(stderr, "\033[91mFAIL:\033[0m ");
        vfprintf(stderr, format, args);
    }
    else {
        passed++;
        printf("\033[92mPASS:\033[0m ");
        vprintf(format, args);
    }
    va_end(args);
    printf("\n");
    return b;

/*}}}*/ }

void print_test_report() { //{{{
    unsigned int failed = tested - passed;
    printf("\033[93m%d TESTS\033[0m  /  \033[92m%d PASSED\033[0m", tested, passed);
    if (failed) {
        printf("  /  \033[91m%d FAILED\033[0m", failed);
    }
    printf("\n");
/*}}}*/ }

void tetromino_print_grid(tetromino_t t) { //{{{
    uint16_t grid = tetromino_get_grid(&t);
    printf("%c => %d\n ", t.type, grid);
    uint16_t mask = 1;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (mask & grid) printf("1");
            else printf("0");
            mask <<= 1;
        }
        printf("\n ");
    }
    printf("\n");
/*}}}*/}

void print_binary(uint16_t n) { //{{{
    printf("%d => ", n);
    uint16_t mask = 1<<15;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (mask & n) printf("1");
            else printf("0");
            mask >>= 1;
        }
        printf(" ");
    }
    printf("\n");
/*}}}*/}

void test_tetromino_copy_and_rotate() { //{{{
    for (int ti = 0; ti < 7; ++ti) {
        tetromino_t t = TETROMINOS[ti];

        const char type = tetromino_get_type_char(&t);
        const char type2 = tetromino_get_type_char((tetromino_t *)&(TETROMINOS[ti]));

        if (!assert(t.type == TETROMINOS[ti].type, "%c == %c", type, type2)) continue;

        uint16_t grid,
                 expected_grid,
                 expected_grid_rotation90,
                 expected_grid_rotation180,
                 expected_grid_rotation270;

        switch(t.type) {
            default:
            case I:
                expected_grid = 3840;
                expected_grid_rotation90 = 8738;
                expected_grid_rotation180 = 240;
                expected_grid_rotation270 = 17476;
                break;
            case J:
                expected_grid = 2272;
                expected_grid_rotation90 = 1604;
                expected_grid_rotation180 = 226;
                expected_grid_rotation270 = 1100;
                break;
            case L:
                expected_grid = 736;
                expected_grid_rotation90 = 1094;
                expected_grid_rotation180 = 232;
                expected_grid_rotation270 = 3140;
                break;
            case O:
                expected_grid = 1632;
                expected_grid_rotation90 = 1632;
                expected_grid_rotation180 = 1632;
                expected_grid_rotation270 = 1632;
                break;
            case S:
                expected_grid = 1728;
                expected_grid_rotation90 = 1122;
                expected_grid_rotation180 = 108;
                expected_grid_rotation270 = 2244;
                break;
            case T:
                expected_grid = 1248;
                expected_grid_rotation90 = 1124;
                expected_grid_rotation180 = 228;
                expected_grid_rotation270 = 1220;
                break;
            case Z:
                expected_grid = 3168;
                expected_grid_rotation90 = 612;
                expected_grid_rotation180 = 198;
                expected_grid_rotation270 = 1224;
                break;
        }

        grid = tetromino_get_grid(&t);
        assert(grid == expected_grid,
               "%c initial grid %d == %d", type, grid, expected_grid);

        grid = tetromino_rotate_clockwise(&t);
        assert(grid == expected_grid_rotation90,
               "%c 90deg clockwise %d == %d", type, grid, expected_grid_rotation90);

        grid = tetromino_rotate_clockwise(&t);
        assert(grid == expected_grid_rotation180,
               "%c 180deg clockwise %d == %d", type, grid, expected_grid_rotation180);

        grid = tetromino_rotate_clockwise(&t);
        assert(grid == expected_grid_rotation270,
               "%c 270deg clockwise %d == %d", type, grid, expected_grid_rotation270);

        grid = tetromino_rotate_clockwise(&t);
        assert(grid == expected_grid, "%c 360deg clockwise == initial grid", type);

        grid = tetromino_rotate_counterclockwise(&t);
        assert(grid == expected_grid_rotation270,
               "%c 270deg counter clockwise %d == %d", type, grid, expected_grid_rotation270);

        grid = tetromino_rotate_counterclockwise(&t);
        assert(grid == expected_grid_rotation180,
               "%c 180deg counter clockwise %d == %d", type, grid, expected_grid_rotation180);

        grid = tetromino_rotate_counterclockwise(&t);
        assert(grid == expected_grid_rotation90,
               "%c 90deg counter clockwise %d == %d", type, grid, expected_grid_rotation90);

        grid = tetromino_rotate_counterclockwise(&t);
        assert(grid == expected_grid, "%c 360deg counter clockwise == initial grid", type);

        fprintf(stderr, "\n");
    }
/*}}}*/ }

int main() {
    test_tetromino_copy_and_rotate();
    print_test_report();
    return 0;
}