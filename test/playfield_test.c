#include <stdlib.h>
#include "test.h"
#include "../src/playfield.h"


bool test_playfield_get_4x4_vacancy_at_coordinate(uint8_t x, uint8_t y, uint16_t expected) {
    uint16_t actual = playfield_get_4x4_vacancy_at_coordinate(x,y);
    char actual_bits[17], expected_bits[17];
    format_binary_string(actual_bits, actual);
    format_binary_string(expected_bits, expected);
    return assert(actual == expected,
                  "4x4 @ (%d,%d) = %s  (actual: %s)", x, y, expected_bits, actual_bits);
}


void test_empty_playfield_vacancy_top() { //{{{

    const uint16_t expected_4x4s[][7] = {
        [0] = {
            [0] = (uint16_t)0b1111111111111110,
            [1] = (uint16_t)0b1111111111111100,
            [2] = (uint16_t)0b1111111111111000,
            [3] = (uint16_t)0b1111111111110000,
            /*...*/
            [4] = (uint16_t)0b1111111111110001,
            [5] = (uint16_t)0b1111111111110011,
            [6] = (uint16_t)0b1111111111110111
        },
        [1] = {
            [0] = (uint16_t)0b1111111111101110,
            [1] = (uint16_t)0b1111111111001100,
            [2] = (uint16_t)0b1111111110001000,
            [3] = (uint16_t)0b1111111100000000,
            /*...*/
            [4] = (uint16_t)0b1111111100010001,
            [5] = (uint16_t)0b1111111100110011,
            [6] = (uint16_t)0b1111111101110111
        },
        [2] = {
            [0] = (uint16_t)0b1111111011101110,
            [1] = (uint16_t)0b1111110011001100,
            [2] = (uint16_t)0b1111100010001000,
            [3] = (uint16_t)0b1111000000000000,
            /*...*/
            [4] = (uint16_t)0b1111000100010001,
            [5] = (uint16_t)0b1111001100110011,
            [6] = (uint16_t)0b1111011101110111
        },
        [3] = {
            [0] = (uint16_t)0b1110111011101110,
            [1] = (uint16_t)0b1100110011001100,
            [2] = (uint16_t)0b1000100010001000,
            [3] = (uint16_t)0b0000000000000000,
            /*...*/
            [4] = (uint16_t)0b0001000100010001,
            [5] = (uint16_t)0b0011001100110011,
            [6] = (uint16_t)0b0111011101110111
        },
    };

    for (int y = 0; y < 4; ++y) {

        uint8_t x = 0;
        for (; x < 4; ++x) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, y, expected_4x4s[y][x]);
        }

        for (; x < PLAYFIELD_WIDTH; ++x) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, y, expected_4x4s[y][3]);
        }

        uint8_t x2=4;
        for (; x < PLAYFIELD_WIDTH+3; ++x, ++x2) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, y, expected_4x4s[y][x2]);
        }

    }

/*}}}*/ }


void test_empty_playfield_vacancy_bottom() { //{{{

    const uint16_t expected_4x4s[][7] = {
        [0] = {
            [0] = (uint16_t)0b1110111011101110,
            [1] = (uint16_t)0b1100110011001100,
            [2] = (uint16_t)0b1000100010001000,
            [3] = (uint16_t)0b0000000000000000,
            /*...*/
            [4] = (uint16_t)0b0001000100010001,
            [5] = (uint16_t)0b0011001100110011,
            [6] = (uint16_t)0b0111011101110111
        },
        [1] = {
            [0] = (uint16_t)0b1110111011101111,
            [1] = (uint16_t)0b1100110011001111,
            [2] = (uint16_t)0b1000100010001111,
            [3] = (uint16_t)0b0000000000001111,
            /*...*/
            [4] = (uint16_t)0b0001000100011111,
            [5] = (uint16_t)0b0011001100111111,
            [6] = (uint16_t)0b0111011101111111
        },
        [2] = {
            [0] = (uint16_t)0b1110111011111111,
            [1] = (uint16_t)0b1100110011111111,
            [2] = (uint16_t)0b1000100011111111,
            [3] = (uint16_t)0b0000000011111111,
            /*...*/
            [4] = (uint16_t)0b0001000111111111,
            [5] = (uint16_t)0b0011001111111111,
            [6] = (uint16_t)0b0111011111111111
        },
        [3] = {
            [0] = (uint16_t)0b1110111111111111,
            [1] = (uint16_t)0b1100111111111111,
            [2] = (uint16_t)0b1000111111111111,
            [3] = (uint16_t)0b0000111111111111,
            /*...*/
            [4] = (uint16_t)0b0001111111111111,
            [5] = (uint16_t)0b0011111111111111,
            [6] = (uint16_t)0b0111111111111111
        }
    };

    for (int y = 0; y < 4; ++y) {

        uint8_t _y = (PLAYFIELD_HEIGHT_1) + y;

        uint8_t x = 0;
        for (; x < 4; ++x) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, _y, expected_4x4s[y][x]);
        }

        for (; x < PLAYFIELD_WIDTH; ++x) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, _y, expected_4x4s[y][3]);
        }

        uint8_t x2=4;
        for (; x < PLAYFIELD_WIDTH+3; ++x, ++x2) {
            test_playfield_get_4x4_vacancy_at_coordinate(x, _y, expected_4x4s[y][x2]);
        }

    }

/*}}}*/ }



int main() {
    test_empty_playfield_vacancy_top();
    test_empty_playfield_vacancy_bottom();
    print_test_report();
    return 0;
}