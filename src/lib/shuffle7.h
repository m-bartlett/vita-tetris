#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <stdint.h>

void bag_of_7_init(int seed);
const uint8_t bag_of_7_pop_sample(void);
void bag_of_7_write_queue(uint8_t *queue, uint8_t queue_length);

#endif