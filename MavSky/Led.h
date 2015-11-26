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
#ifndef LED_H
#define LED_H

#include <WProgram.h> 
#include "OctoWS2811.h"

#define LED_MAX_STRIPS              8
#define LED_MAX_BULBS               8
#define LED_MAX_STRIP_STATES        10
#define LED_MAX_PATTERNS_PER_STRIP  25
#define LED_DEFAULT_STATE_TIME      200

class LedBulbColor {
  public:
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    LedBulbColor(uint8_t red_param, uint8_t green_param, uint8_t blue_param);
};

class LedStripState {
  public:
    LedBulbColor* bulbs[LED_MAX_BULBS];  
    uint16_t state_time = 0; 
    LedStripState();
    LedStripState(uint16_t time_param);
    LedStripState(LedBulbColor* c1, LedBulbColor* c2, LedBulbColor* c3, LedBulbColor* c4, LedBulbColor* c5, LedBulbColor* c6, LedBulbColor* c7, LedBulbColor* c8, uint16_t time_param);
};

class LedStripPattern {
  private:
    char*     pattern_name;

  public:
    uint8_t   strip_state_count = 0;

    LedStripState* led_strip_states[LED_MAX_STRIP_STATES];
    LedStripPattern(char* name_param);
    void add_strip_state(LedStripState* strip_state_param);
    void add_strip_state(LedBulbColor* c1, LedBulbColor* c2, LedBulbColor* c3, LedBulbColor* c4, LedBulbColor* c5, LedBulbColor* c6, LedBulbColor* c7, LedBulbColor* c8, uint16_t time_param);
};

class LedStrip {
  public:
    uint8_t  current_state = 0;
    uint32_t current_state_expiry_time = 0L;
    LedStrip();
    LedStripPattern* led_patterns[LED_MAX_PATTERNS_PER_STRIP];  
};

class LedController {
  private:
    OctoWS2811* leds;
 
    uint8_t leds_on_strip = 0;
    void add_pattern(int pattern_number, int strip_number, LedStripPattern* pattern);
    void change_led_state(int strip_number, LedStripPattern* pattern, LedStrip* strip_ptr);
    
  public:
    LedStrip* led_strips[LED_MAX_STRIPS];  
    LedController();
    void process_10_millisecond();
};

#endif


