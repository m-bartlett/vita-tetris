#ifndef TIMER_H
#define TIMER_H

#include <vitasdk.h>

typedef SceUInt64 vita_timestamp_t; 
vita_timestamp_t timer_get_elapsed_microseconds(vita_timestamp_t A, vita_timestamp_t B);
vita_timestamp_t timer_get_current_time();

#endif