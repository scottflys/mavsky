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
#include "LedGroupAction.h"

#define MAX_LEDS_PER_GROUP 64
#define MAX_LED_GROUPS     20      // scs
#define MAX_LED_LAYERS     50      

class LedGroups;

class LedGroup {
  private:
    LedGroups* led_groups_ptr;
    int group_number;
    
  public: 
    LedGroupActions* group_actions_ptr;

    uint8_t  led_count = 0;
    int8_t   strip_number[MAX_LEDS_PER_GROUP];
    int8_t   led_position[MAX_LEDS_PER_GROUP];
    LedGroup(OctoWS2811* led_ptr, LedGroups*, int group_number_param);
    void clear_led_assignments();
    void push_layer(uint8_t action_number);

};

class LedGroups {
  private:
    LedGroup* group_ptrs[MAX_LED_GROUPS];
    uint16_t layers[MAX_LED_LAYERS];
    uint16_t led_layer_count = 0;    
    
  public:
    uint8_t led_group_count = MAX_LED_GROUPS;
    LedGroups(OctoWS2811* led_ptr);
    void add_led_to_group(uint8_t group_number, uint8_t strip, uint8_t pos);
    LedGroup* get_led_group(uint8_t group_number);    
    void clear_all_led_assignments();
    void process_10_milliseconds();
    void disable_all_actions();
    void push_layer(uint8_t group_number, uint8_t action_number);

};

#endif


