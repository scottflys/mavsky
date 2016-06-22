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
#ifndef LEDGROUPACTION_H
#define LEDGROUPACTION_H

#include <WProgram.h> 
#include "FastLED.h"

class LedGroupActions;
class LedGroup;

class LedGroupAction {
  private:
    CRGB* leds;
    uint8_t group_number;
    LedGroup* group_ptr;

  public:
    uint16_t pc = 0;
    uint8_t enabled = 0;
    uint8_t in_use = 0;
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
    uint32_t offset_time = 0;                         // flash2
    uint32_t pause_time = 0;                          // fill
    
    LedGroupAction(CRGB led_ptr[], LedGroup*);
    void set_led(int on_led, uint32_t color);
    void init(uint16_t pc_param, uint8_t mode_param, uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param, uint32_t state_time_param, uint32_t on_width_param, uint32_t pause_time_param, uint8_t intensity_param, uint8_t on_led_count_param, uint8_t reverse_param);    void process_10_millisecond();
    void disable();
    void force_off();
    void clear();
    void dump_diags();
};

class LedGroupActions {
  private:
    LedGroup* led_group_ptr;
    
  public:
    LedGroupAction* group_action_ptrs[MAX_LED_ACTIONS_PER_GROUP];
    LedGroupActions(CRGB led_ptr[], LedGroup* group_ptr_param);
    void push_layer(uint8_t group_number);
    void disable_all_actions();
    LedGroupAction* get_led_group(uint8_t group_number);        /// todo this isn't named right
    void dump_diags();
    int get_next_empty_action();
    void clear_all_actions();
    void set_solid(uint16_t pc, uint32_t color);
    void set_flash(uint16_t pc, uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param);
    void set_wave(uint16_t pc, uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param, uint8_t reverse_param);
    void set_bounce(uint16_t pc, uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param);
    void set_random(uint16_t pc, uint32_t state_time_param, uint8_t intensity);
    void set_bar(uint16_t pc, uint32_t on_color_param, uint32_t percent_register_param, uint8_t reverse_param);  
    void set_fill(uint16_t pc, uint32_t on_color_param, uint32_t state_time_param, uint32_t pause_time_param, uint8_t reverse_param);  
};

#endif


