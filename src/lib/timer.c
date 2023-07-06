#include "timer.h"

uint32_t timer_get_elapsed_microseconds(vita_timestamp_t *A, vita_timestamp_t *B)
{
    return (*B)-(*A);
}


void timer_unset(vita_timestamp_t *timer)
{
    *timer = 0;
}


bool timer_is_null(vita_timestamp_t *timer)
{
    return (*timer) == 0;
}


vita_timestamp_t timer_get_current_time()
{
    return sceKernelGetProcessTimeWide();
}

void timer_set_current_time(vita_timestamp_t *timer)
{
    *timer = timer_get_current_time();
}