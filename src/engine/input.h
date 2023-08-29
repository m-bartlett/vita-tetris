#ifndef INPUT_H
#define INPUT_H

#include "core.h"
#include "../lib/timer.h"
#include "../lib/log2.h"

/*
    SCE_CTRL_SELECT   == 0x00000001 == 0b0000000000000001
    SCE_CTRL_L3       == 0x00000002 == 0b0000000000000010
    SCE_CTRL_R3       == 0x00000004 == 0b0000000000000100
    SCE_CTRL_START    == 0x00000008 == 0b0000000000001000
    SCE_CTRL_UP       == 0x00000010 == 0b0000000000010000
    SCE_CTRL_RIGHT    == 0x00000020 == 0b0000000000100000
    SCE_CTRL_DOWN     == 0x00000040 == 0b0000000001000000
    SCE_CTRL_LEFT     == 0x00000080 == 0b0000000010000000
    SCE_CTRL_L2       == 0x00000100 == 0b0000000100000000
    SCE_CTRL_R2       == 0x00000200 == 0b0000001000000000
    SCE_CTRL_L1       == 0x00000400 == 0b0000010000000000
    SCE_CTRL_R1       == 0x00000800 == 0b0000100000000000
    SCE_CTRL_TRIANGLE == 0x00001000 == 0b0001000000000000
    SCE_CTRL_CIRCLE   == 0x00002000 == 0b0010000000000000
    SCE_CTRL_CROSS    == 0x00004000 == 0b0100000000000000
    SCE_CTRL_SQUARE   == 0x00008000 == 0b1000000000000000
*/


// reference button enums with lower-case button names
#define INPUT_BITMASK_up SCE_CTRL_UP
#define INPUT_BITMASK_down SCE_CTRL_DOWN
#define INPUT_BITMASK_left SCE_CTRL_LEFT
#define INPUT_BITMASK_right SCE_CTRL_RIGHT
#define INPUT_BITMASK_triangle SCE_CTRL_TRIANGLE
#define INPUT_BITMASK_circle SCE_CTRL_CIRCLE
#define INPUT_BITMASK_cross SCE_CTRL_CROSS
#define INPUT_BITMASK_square SCE_CTRL_SQUARE
#define INPUT_BITMASK_l1 SCE_CTRL_L1
#define INPUT_BITMASK_r1 SCE_CTRL_R1
#define INPUT_BITMASK_l2 SCE_CTRL_L2
#define INPUT_BITMASK_r2 SCE_CTRL_R2
#define INPUT_BITMASK_l3 SCE_CTRL_L3
#define INPUT_BITMASK_r3 SCE_CTRL_R3
#define INPUT_BITMASK_select SCE_CTRL_SELECT
#define INPUT_BITMASK_start SCE_CTRL_START

#define INPUT_BITPOS_up (LOG_2(INPUT_BITMASK_up))
#define INPUT_BITPOS_down (LOG_2(INPUT_BITMASK_down))
#define INPUT_BITPOS_left (LOG_2(INPUT_BITMASK_left))
#define INPUT_BITPOS_right (LOG_2(INPUT_BITMASK_right))
#define INPUT_BITPOS_triangle (LOG_2(INPUT_BITMASK_triangle))
#define INPUT_BITPOS_circle (LOG_2(INPUT_BITMASK_circle))
#define INPUT_BITPOS_cross (LOG_2(INPUT_BITMASK_cross))
#define INPUT_BITPOS_square (LOG_2(INPUT_BITMASK_square))
#define INPUT_BITPOS_l1 (LOG_2(INPUT_BITMASK_l1))
#define INPUT_BITPOS_r1 (LOG_2(INPUT_BITMASK_r1))
#define INPUT_BITPOS_l2 (LOG_2(INPUT_BITMASK_l2))
#define INPUT_BITPOS_r2 (LOG_2(INPUT_BITMASK_r2))
#define INPUT_BITPOS_l3 (LOG_2(INPUT_BITMASK_l3))
#define INPUT_BITPOS_r3 (LOG_2(INPUT_BITMASK_r3))
#define INPUT_BITPOS_select (LOG_2(INPUT_BITMASK_select))
#define INPUT_BITPOS_start (LOG_2(INPUT_BITMASK_start))

#define INPUT_BITPOS_analog_left 0
#define INPUT_BITPOS_analog_right 1

#define INPUT_MAX_BITMASK SCE_CTRL_SQUARE
#define INPUT_MAX_BITSHIFT (LOG_2(INPUT_MAX_BITMASK))
#define INPUT_BITFIELD_WIDTH (INPUT_MAX_BITSHIFT+1)


extern vita_timestamp_t button_debounces[INPUT_BITFIELD_WIDTH];
extern void (*button_callbacks[INPUT_BITFIELD_WIDTH])(void);
extern void (*analog_callbacks[2])(unsigned char, unsigned char);

#define input_set_button_callback(button, callback) \
   button_callbacks[INPUT_BITPOS_ ## button] = callback

#define input_set_analog_callback(analog_stick, callback) \
   analog_callbacks[INPUT_BITPOS_ ## analog_stick] = callback

void input_init();
void input_read_and_run_callbacks();

#endif