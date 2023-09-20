#include <vitasdk.h>
#include "input.h"


static SceCtrlData input, previous_input;

static void NULL_FUNC(void){};
static void NULL_FUNC_ANALOG(unsigned char x, unsigned char y){};

vita_timestamp_t button_debounces[INPUT_BITFIELD_WIDTH] = {0};
void (*button_callbacks[INPUT_BITFIELD_WIDTH])(void) = {NULL_FUNC};
void (*analog_callbacks[2])(unsigned char, unsigned char) = {NULL_FUNC_ANALOG};


void input_init() {
   sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG); // don't need *_WIDE analog
}


#define check_button_held_repeats(button, input_debounce_microseconds) \
   if (input.buttons & INPUT_BITMASK_ ## button) { \
      vita_timestamp_t now = timer_get_current_time(); \
      if (now - button_debounces[INPUT_BITPOS_ ## button] > input_debounce_microseconds) { \
         button_callbacks[INPUT_BITPOS_ ## button](); \
         button_debounces[INPUT_BITPOS_ ## button] = now; \
      } \
   } \
   else { \
      button_debounces[INPUT_BITPOS_ ## button] = 0; \
   }


#define check_button_single_fire(button) \
   if ( (!(previous_input.buttons & INPUT_BITMASK_ ## button)) \
        && (input.buttons & INPUT_BITMASK_ ## button) ) { \
      button_callbacks[INPUT_BITPOS_ ## button](); \
   }

 
void input_read_and_run_callbacks()
{
   sceCtrlPeekBufferPositive(0, &input, 1);
   
   check_button_single_fire(up)
   check_button_held_repeats(right, /*debounce_microseconds=*/68500)
   check_button_held_repeats(left,  /*debounce_microseconds=*/68500)
   check_button_held_repeats(down,  /*debounce_microseconds=*/35000)

   check_button_single_fire(triangle)
   check_button_single_fire(circle)
   check_button_single_fire(cross)
   check_button_single_fire(square)

   check_button_single_fire(l2)
   // check_button_single_fire(l2)
   // check_button_single_fire(l3)
   check_button_single_fire(r2)
   // check_button_single_fire(r2)
   // check_button_single_fire(r3)

   check_button_single_fire(select)
   check_button_single_fire(start)

   analog_callbacks[0](input.lx, input.ly);
   analog_callbacks[1](input.rx, input.ry);

   previous_input = input;
}