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
#include <Adafruit_NeoPixel.h>

#define LED_MAX_BULBS         8
#define LED_MAX_STRIP_STATES  10
#define LED_MAX_PATTERNS      25
#define LED_DEFAULT_STATE_TIME 200

class LedBulbState {
  public:
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    LedBulbState(uint8_t red_param, uint8_t green_param, uint8_t blue_param);
};

class LedStripState {
  public:
    uint8_t bulb_count = 0;
    LedBulbState* bulbs[LED_MAX_BULBS];  
    uint16_t state_time = 0; 
    
    void add_bulb(uint8_t red_param, uint8_t green_param, uint8_t blue_param);
    void add_bulbs(uint8_t red_param, uint8_t green_param, uint8_t blue_param, uint8_t count);
    LedStripState();
    LedStripState(uint16_t time_param);
};

class LedPattern {
  private:
    char*     pattern_name;

  public:
    uint8_t   strip_state_count = 0;

    LedStripState* led_strip_states[LED_MAX_STRIP_STATES];
    LedPattern(char* name_param);
    void add_strip_state(LedStripState* strip_state_param);
};

class Led {
  private:
    Adafruit_NeoPixel* strip; 
    uint16_t led_pattern_count = 0;  
    uint8_t leds_on_strip = 0;

    void add_pattern(LedPattern* pattern);
    void show_pattern(uint8_t pattern_index, uint8_t reverse);

  public:
    LedPattern* led_patterns[LED_MAX_PATTERNS];  
    Led(uint8_t pin, uint8_t led_count);
    void process_100_millisecond();
};

#endif


