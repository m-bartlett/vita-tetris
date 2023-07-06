#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>
#include <vitasdk.h>

typedef SceUInt64 vita_timestamp_t; 

uint32_t timer_get_elapsed_microseconds(vita_timestamp_t *A, vita_timestamp_t *B);
void timer_unset(vita_timestamp_t *timer);
bool timer_is_null(vita_timestamp_t *timer);
void timer_set_current_time(vita_timestamp_t *timer);
vita_timestamp_t timer_get_current_time();


#endif