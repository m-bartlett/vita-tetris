#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "../src/playfield.h"


bool test_playfield_get_4x4_vacancy_at_coordinate(uint8_t X, uint8_t Y, uint16_t expected) {
    uint16_t actual = playfield_get_4x4_vacancy_at_coordinate(X,Y);
    char actual_bits[17], expected_bits[17];
    format_binary_string(actual_bits, actual);
    format_binary_string(expected_bits, expected);
    return assert(actual == expected,
                  "vacancy 4x4 @ (%d,%d) ==\t%s  (actual: %s)", X, Y, expected_bits, actual_bits);
}

bool assert_playfield_grid_at_coordinate(uint8_t X, uint8_t Y, const char* expected) {
    const uint8_t X1=X+1, Y1=Y+1;
    char actual[17] = {0};
    playfield_view_t view = playfield_view();
    char* actual_i = actual;
    
    for (int8_t y=Y-3; y < Y1; ++y) {
        if (y < 0 || y > PLAYFIELD_HEIGHT_1) {
            for (uint8_t _i = 0; _i < 4; ++_i) (*actual_i++) = '#';
            continue;
        }
        for (int8_t x=X-3; x < X1; ++x) {
            if (x < 0 || x > PLAYFIELD_WIDTH_1) (*actual_i++) = '#';
            else {
                char symbol;
                int8_t block = view[y][x];
                if (block) symbol = tetromino_type_t2char((enum tetromino_type_t)block);
                else symbol=' ';
                (*actual_i++) = symbol;
            }
        }
    }

    return assert(strcmp(expected, actual) == 0,
                  "playfield 4x4 view @ (%d,%d) == \"%s\"  (actual: \"%s\")",
                  X, Y, expected, actual);
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


void test_playfield_tetromino_placement() { /*{{{*/
    
    /* Basic tetromino placement and playfield block-type window tests {{{ */

    tetromino_t z = {Z, 0};

    assert(playfield_validate_tetromino_placement(&z, 4, 4) == true,
           "placing Z piece on empty squares is valid");

    playfield_place_tetromino(&z, 4, 4);

    assert_playfield_grid_at_coordinate(4, 0, "####"
                                              "####"
                                              "####"
                                              "    ");

    assert_playfield_grid_at_coordinate(4, 1, "####"
                                              "####"
                                              "    "
                                              "    ");

    assert_playfield_grid_at_coordinate(4, 2, "####"
                                              "    "
                                              "    "
                                              "ZZ  ");

    assert_playfield_grid_at_coordinate(4, 3, "    "
                                              "    "
                                              "ZZ  "
                                              " ZZ ");

    assert_playfield_grid_at_coordinate(4, 3, "    "
                                              "    "
                                              "ZZ  "
                                              " ZZ ");

    assert_playfield_grid_at_coordinate(4, PLAYFIELD_HEIGHT, "    "
                                                             "    "
                                                             "    "
                                                             "####");

    assert_playfield_grid_at_coordinate(4, PLAYFIELD_HEIGHT+1, "    "
                                                               "    "
                                                               "####"
                                                               "####");

    assert_playfield_grid_at_coordinate(4, PLAYFIELD_HEIGHT+2, "    "
                                                               "####"
                                                               "####"
                                                               "####");

    assert_playfield_grid_at_coordinate(4, PLAYFIELD_HEIGHT+3, "####"
                                                               "####"
                                                               "####"
                                                               "####");

    assert_playfield_grid_at_coordinate(4, PLAYFIELD_HEIGHT+4, "####"
                                                               "####"
                                                               "####"
                                                               "####");

    /* }}} */

    /* Test valid placements (i.e. tetromino can only be placed inside empty blocks)
        as well as further playfield block-type window tests when forcing tetromino placement.
        {{{
    */
    tetromino_rotate_clockwise(&z);

    assert(playfield_validate_tetromino_placement(&z, 4, 8) == true,
           "placing rotated Z piece on empty squares is valid");

    playfield_place_tetromino(&z, 4, 8);
    assert_playfield_grid_at_coordinate(4, 8, "    "
                                              "  Z "
                                              " ZZ "
                                              " Z  ");
    
    tetromino_t l = {L, 0};

    assert(playfield_validate_tetromino_placement(&l, 4, 4) == false,
           "placing L piece on top of Z piece is invalid");

    playfield_place_tetromino(&l, 4, 4);
    assert_playfield_grid_at_coordinate(4, 4, "    "
                                              "ZZL "
                                              "LLL "
                                              "    ");

    tetromino_rotate_clockwise(&l);

    assert(playfield_validate_tetromino_placement(&l, 4, 8) == false,
           "placing rotated L piece on top of rotated Z piece is invalid");

    playfield_place_tetromino(&l, 4, 8);
    assert_playfield_grid_at_coordinate(4, 8, "    "
                                              " LZ "
                                              " LZ "
                                              " LL ");
    /* }}} */

    /* Test placement validity near playfield edges (literal edge-cases) {{{ */


    tetromino_t j = {J, 2}; // ¬ shape to poke gradually out of bounds on the left

    assert(playfield_validate_tetromino_placement(&j, 2, 12) == false,
           "placing piece slightly on top of left-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(2, 12, "#   "
                                               "#   "
                                               "#   "
                                               "#   ");

    assert(playfield_validate_tetromino_placement(&j, 1, 12) == false,
           "placing piece slightly more on top of left-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(1, 12, "##  "
                                               "##  "
                                               "##  "
                                               "##  ");

    assert(playfield_validate_tetromino_placement(&j, 0, 12) == false,
           "placing piece mostly left-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(0, 12, "### "
                                               "### "
                                               "### "
                                               "### ");

    assert(playfield_validate_tetromino_placement(&j, -1, 12) == false,
           "placing piece completely left-side out-of-bounds is invalid");
    /*
        NOTE: the -1 here is probably wrong but the grid at 255 is probably going to look the
        same in most cases
    */
    assert_playfield_grid_at_coordinate(-1, 12, "####"
                                                "####"
                                                "####"
                                                "####");


    tetromino_rotate_counterclockwise(&j); // ſ shape to poke gradually out of bounds at bottom

    assert(playfield_validate_tetromino_placement(&j, 5, PLAYFIELD_HEIGHT) == false,
           "placing piece slightly on top of bottom-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(5, PLAYFIELD_HEIGHT, "    "
                                                             "    "
                                                             "    "
                                                             "####");

    assert(playfield_validate_tetromino_placement(&j, 5, PLAYFIELD_HEIGHT+1) == false,
           "placing piece slightly more on top of bottom-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(5, PLAYFIELD_HEIGHT+1, "    "
                                                               "    "
                                                               "####"
                                                               "####");

    assert(playfield_validate_tetromino_placement(&j, 5, PLAYFIELD_HEIGHT+2) == false,
           "placing piece mostly bottom-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(5, PLAYFIELD_HEIGHT+2, "    "
                                                               "####"
                                                               "####"
                                                               "####");

    assert(playfield_validate_tetromino_placement(&j, 5, PLAYFIELD_HEIGHT+3) == false,
           "placing piece completely bottom-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(5, PLAYFIELD_HEIGHT+3, "####"
                                                               "####"
                                                               "####"
                                                               "####");


    tetromino_rotate_counterclockwise(&j); // ⌙ shape to poke gradually right-side out of bounds 

    assert(playfield_validate_tetromino_placement(&j, PLAYFIELD_WIDTH, 12) == true,
           "placing this piece adjacent to right-side out-of-bounds is valid");
    assert_playfield_grid_at_coordinate(PLAYFIELD_WIDTH, 12, "   #"
                                                             "   #"
                                                             "   #"
                                                             "   #");

    assert(playfield_validate_tetromino_placement(&j, PLAYFIELD_WIDTH+1, 12) == false,
           "placing piece slightly on top of right-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(PLAYFIELD_WIDTH+1, 12, "  ##"
                                                               "  ##"
                                                               "  ##"
                                                               "  ##");

    assert(playfield_validate_tetromino_placement(&j, PLAYFIELD_WIDTH+2, 12) == false,
           "placing piece mostly right-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(PLAYFIELD_WIDTH+2, 12, " ###"
                                                               " ###"
                                                               " ###"
                                                               " ###");

    assert(playfield_validate_tetromino_placement(&j, PLAYFIELD_WIDTH+3, 12) == false,
           "placing piece completely right-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(PLAYFIELD_WIDTH+3, 12, "####"
                                                               "####"
                                                               "####"
                                                               "####");


    tetromino_rotate_counterclockwise(&j); // ˩ shape to poke gradually top-side out of bounds 

    assert(playfield_validate_tetromino_placement(&j, 9, 3) == true,
           "placing this piece adjacent to top-side out-of-bounds is valid");
    assert_playfield_grid_at_coordinate(9, 3, "    "
                                              "    "
                                              "    "
                                              "    ");

    assert(playfield_validate_tetromino_placement(&j, 9, 2) == true,
           "placing this piece slightly on top of top-side out-of-bounds is valid");
    assert_playfield_grid_at_coordinate(9, 2, "####"
                                              "    "
                                              "    "
                                              "    ");

    assert(playfield_validate_tetromino_placement(&j, 9, 1) == false,
           "placing piece slightly more on top of top-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(9, 1, "####"
                                              "####"
                                              "    "
                                              "    ");

    assert(playfield_validate_tetromino_placement(&j, 9, 0) == false,
           "placing piece mostly top-side out-of-bounds is still invalid");
    assert_playfield_grid_at_coordinate(9, 0, "####"
                                              "####"
                                              "####"
                                              "    ");

    assert(playfield_validate_tetromino_placement(&j, 9, -1) == false,
           "placing piece completely top-side out-of-bounds is invalid");
    assert_playfield_grid_at_coordinate(9, -1, "####"
                                               "####"
                                               "####"
                                               "####");

    /* }}} */

/*}}}*/ }


int main() {
    test_empty_playfield_vacancy_top();
    test_empty_playfield_vacancy_bottom();
    test_playfield_tetromino_placement();
    print_test_report();
    return 0;
}