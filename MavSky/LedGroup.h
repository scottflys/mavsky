//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  A copy of the GNU General Public License is available at <http://www.gnu.org/licenses/>.
//  
#ifndef LEDGROUP_H
#define LEDGROUP_H

#include <WProgram.h> 
#include "OctoWS2811.h"

#define MAX_LEDS_PER_GROUP 64
#define MAX_LED_GROUPS     20

class LedGroups;

class LedGroup {
  private:
    OctoWS2811* leds;
    LedGroups* led_groups_ptr;
    uint8_t group_number;
    uint8_t enabled = 0;
  public:
    uint32_t sequence_ms = 0;
    uint8_t  state = 0;
    uint8_t  mode = 0;    
    uint32_t on_color = 0;
    uint32_t on_time = 0;                             // flash
    uint32_t off_time = 0;                            // flash
    uint32_t state_time = 0;  
    uint8_t  on_width = 0;                            // wave
    uint8_t  on_led_count = 0;                        // bar
    uint8_t  intensity = 0;                           // random
    uint8_t  reverse = 0;                             // wave, bar
    uint32_t offset_time = 0;                             // flash2
    
    uint8_t  led_count = 0;
    int8_t   strip_number[MAX_LEDS_PER_GROUP];
    int8_t   led_position[MAX_LEDS_PER_GROUP];
    LedGroup(OctoWS2811* led_ptr, LedGroups* groups, uint8_t group_number);
    void push_layer();
    void process_10_millisecond();
    void clear();
    void disable();
    void set_solid(uint32_t color);
    void set_dormant();
    void set_flash(uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param);
    void set_wave(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param, uint8_t reverse_param);
    void set_bounce(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param);
    void set_random(uint32_t state_time_param, uint8_t intensity);
    void set_bar(uint32_t on_color_param, uint32_t percent_register_param, uint8_t reverse_param);
    void force_off();
};

class LedGroups {
  private:
    int8_t layers[MAX_LED_GROUPS];
  public:
    LedGroup* group_ptrs[MAX_LED_GROUPS];
    uint8_t   led_group_count = MAX_LED_GROUPS;
    LedGroups(OctoWS2811* led_ptr);
    void push_layer(uint8_t group_number);
    void add_led_to_group(uint8_t group_number, uint8_t strip, uint8_t pos);
    void clear_all();
    void disable_all();
    LedGroup* get_led_group(uint8_t group_number);
    void process_10_milliseconds();
};

#endif


