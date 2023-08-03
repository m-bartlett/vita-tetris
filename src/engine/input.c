#include "input.h"
#include <vitasdk.h>

static SceCtrlData input, debounce;

static void NULL_FUNC(void){};
static void NULL_FUNC_ANALOG(unsigned char x, unsigned char y){};

#define DECLARE_BUTTON_CALLBACK_AND_SETTER(BUTTON) \
   static void (*input_callback_ ## BUTTON)(void) = NULL_FUNC; \
   void input_set_callback_ ## BUTTON(void(*callback)(void)) {\
      input_callback_ ## BUTTON = callback;\
   }

#define DECLARE_ANALOG_CALLBACK_AND_SETTER(STICK) \
   static void (*input_callback_analog_ ## STICK)(unsigned char, unsigned char)=NULL_FUNC_ANALOG; \
   void input_set_callback_analog_ ## STICK(void(*callback)(unsigned char, unsigned char)) {\
      input_callback_analog_ ## STICK = callback;\
   }

DECLARE_BUTTON_CALLBACK_AND_SETTER(up)
DECLARE_BUTTON_CALLBACK_AND_SETTER(right)
DECLARE_BUTTON_CALLBACK_AND_SETTER(down)
DECLARE_BUTTON_CALLBACK_AND_SETTER(left)

DECLARE_BUTTON_CALLBACK_AND_SETTER(triangle)
DECLARE_BUTTON_CALLBACK_AND_SETTER(circle)
DECLARE_BUTTON_CALLBACK_AND_SETTER(cross)
DECLARE_BUTTON_CALLBACK_AND_SETTER(square)

DECLARE_BUTTON_CALLBACK_AND_SETTER(l1)
DECLARE_BUTTON_CALLBACK_AND_SETTER(l2)
DECLARE_BUTTON_CALLBACK_AND_SETTER(l3)
DECLARE_BUTTON_CALLBACK_AND_SETTER(r1)
DECLARE_BUTTON_CALLBACK_AND_SETTER(r2)
DECLARE_BUTTON_CALLBACK_AND_SETTER(r3)

DECLARE_BUTTON_CALLBACK_AND_SETTER(select)
DECLARE_BUTTON_CALLBACK_AND_SETTER(start)

DECLARE_ANALOG_CALLBACK_AND_SETTER(left)
DECLARE_ANALOG_CALLBACK_AND_SETTER(right)


void input_init() {
   sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
}

#define IS_PRESSED_HOLDABLE(button) (input.buttons & button)
#define IS_PRESSED(button) ((!(debounce.buttons & button)) && (input.buttons & button))
 
void input_read_and_run_callbacks()
{
   sceCtrlPeekBufferPositive(0, &input, 1);
   
   if (IS_PRESSED(SCE_CTRL_UP))       input_callback_up();
   if (IS_PRESSED_HOLDABLE(SCE_CTRL_RIGHT))    input_callback_right();
   if (IS_PRESSED_HOLDABLE(SCE_CTRL_DOWN))     input_callback_down();
   if (IS_PRESSED_HOLDABLE(SCE_CTRL_LEFT))     input_callback_left();

   if (IS_PRESSED(SCE_CTRL_TRIANGLE)) input_callback_triangle();
   if (IS_PRESSED(SCE_CTRL_CIRCLE))   input_callback_circle();
   if (IS_PRESSED(SCE_CTRL_CROSS))    input_callback_cross();
   if (IS_PRESSED(SCE_CTRL_SQUARE))   input_callback_square();

   if (IS_PRESSED(SCE_CTRL_L1))       input_callback_l1();
   if (IS_PRESSED(SCE_CTRL_L2))       input_callback_l2();
   if (IS_PRESSED(SCE_CTRL_L3))       input_callback_l3();
   if (IS_PRESSED(SCE_CTRL_R1))       input_callback_r1();
   if (IS_PRESSED(SCE_CTRL_R2))       input_callback_r2();
   if (IS_PRESSED(SCE_CTRL_R3))       input_callback_r3();

   if (IS_PRESSED(SCE_CTRL_SELECT))   input_callback_select();
   if (IS_PRESSED(SCE_CTRL_START))    input_callback_start();

   input_callback_analog_left(input.lx, input.ly);
   input_callback_analog_right(input.rx, input.ry);

   debounce = input;
}