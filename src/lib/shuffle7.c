#include "shuffle7.h"
#include <stdlib.h>
#include <stdio.h>
                                    /*    last element is flip-flop index to alternate bag
                                                      ↓                   ↓                   */
static uint8_t bag_of_7_bags[2][8] = {{0,1,2,3,4,5,6, 1}, {0,1,2,3,4,5,6, 0}};
                                    /* ↑ ↑ ↑ ↑ ↑ ↑ ↑       ↑ ↑ ↑ ↑ ↑ ↑ ↑
                                       elements 0-6 are the shuffle-able order to grab pieces */

static uint8_t bag_of_7_index = 0;
static uint8_t *bag_of_7_bag = bag_of_7_bags[0];


static void bag_of_7_swap_current_bag(void) {
  bag_of_7_bag = bag_of_7_bags[bag_of_7_bag[7]];
}


void bag_of_7_shuffle_current_bag(void)
{
  // srand((unsigned) time(&t));
  uint8_t t;
  for (int i = 0; i < 7; ++i) {
      uint8_t source = rand() % 7;
      uint8_t target= rand() % 7;
      t = bag_of_7_bag[target];
      bag_of_7_bag[target] = bag_of_7_bag[source];
      bag_of_7_bag[source] = t;
  }
}


void bag_of_7_init(int seed)
{
  srand(seed);
  bag_of_7_index = 0;
  bag_of_7_bag = bag_of_7_bags[0];
  bag_of_7_shuffle_current_bag();
  bag_of_7_swap_current_bag();
  bag_of_7_shuffle_current_bag();
  bag_of_7_swap_current_bag();
}


const uint8_t bag_of_7_pop_sample(void)
{
  if (bag_of_7_index > 6) {  // current bag exhausted, shuffle this bag and then switch
    bag_of_7_shuffle_current_bag();
    bag_of_7_swap_current_bag();
    bag_of_7_index=0;
  }
  uint8_t sample = bag_of_7_bag[bag_of_7_index++];
  return sample;
}


void bag_of_7_write_queue(uint8_t *queue, uint8_t queue_length)
{
  queue_length = queue_length > 14 ? 14 : queue_length;
  uint8_t *other_bag = bag_of_7_bags[bag_of_7_bag[7]];
  
  uint8_t queue_index = 0;
  for (int i = bag_of_7_index; i < 7; ++i) {
      queue[queue_index++] = bag_of_7_bag[i];
  }

  for (int i = 0; queue_index < queue_length; ++i) {
      queue[queue_index++] = other_bag[i];
  }

  return;
}