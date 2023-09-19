#include "timer.h"

vita_timestamp_t timer_get_elapsed_microseconds(vita_timestamp_t A, vita_timestamp_t B) {
    return B-A;
}

vita_timestamp_t timer_get_current_time()
{
    return sceKernelGetProcessTimeWide();
}