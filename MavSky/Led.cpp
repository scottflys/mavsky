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
//  Creator:  Scott Simpson
//  

#include "OctoWS2811.h"
#include "Led.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;
extern int displayMemory[];
extern int drawingMemory[];

LedController::LedController() {
  leds_on_strip = 8;

  for (int i=0; i < 8; i++) {
    led_strips[i] = new LedStrip();
  }

  leds = new OctoWS2811(8, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
  leds->begin();

  for (int i=0; i < 8*8; i++) {
    leds->setPixel(i, 0x000000);
  }
  leds->show();

  LedBulbColor* red = new LedBulbColor(255, 0, 0);
  LedBulbColor* green = new LedBulbColor(0, 255, 0);
  //LedBulbColor* blue = new LedBulbColor(0, 0, 255);
  //LedBulbColor* yellow = new LedBulbColor(255, 255, 0);
  //LedBulbColor* cyan = new LedBulbColor(0, 255, 255);
  //LedBulbColor* magenta = new LedBulbColor(255, 0, 255);
  LedBulbColor* off = new LedBulbColor(0, 0, 0);
  LedBulbColor* white = new LedBulbColor(255, 255, 255);
  
  LedStripPattern* led_pattern = new LedStripPattern((char*)"normal"); 
  led_pattern->add_strip_state(red, off, off, off, off, off, off, green, 0);
  add_pattern(0, 0, led_pattern);

  led_pattern = new LedStripPattern((char*)"all_off"); 
  led_pattern->add_strip_state(off, off, off, off, off, off, off, off, 0);
  add_pattern(1, 0, led_pattern);
  
  led_pattern = new LedStripPattern((char*)"landing1"); 
  led_pattern->add_strip_state(red, white, white, white, white, white, white, green, 0);
  add_pattern(2, 0, led_pattern);
  
  // 0                
  // 1 R             G    
  // 2 R-W         W-G
  // 3 R-W-W-    W-W-G
  // 4 R-W-W-W-W-W-W-G
  led_pattern = new LedStripPattern((char*)"landing2"); 
  led_pattern->add_strip_state(off, off, off, off, off, off, off, off, 600);
  led_pattern->add_strip_state(red, off, off, off, off, off, off, green, 100);
  led_pattern->add_strip_state(red, white, off, off, off, off, white, green, 100);
  led_pattern->add_strip_state(red, white, white, off, off, white, white, green, 100);
  led_pattern->add_strip_state(red, white, white, white, white, white, white, green, 100);
  add_pattern(3, 0, led_pattern);

//  3  1   front goes out to in with red on left
//  2  4   back goes in to out with red on left
  
  led_pattern = new LedStripPattern((char*)"flying1");                      // right front out to in green
  led_pattern->add_strip_state(off, off, off, off, off, off, off, green, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, green, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, green, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, green, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, green, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, green, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, green, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(green, off, off, off, off, off, off, off, 100);
  add_pattern(4, 0, led_pattern);  

  led_pattern = new LedStripPattern((char*)"flying1");                      // left rear in to out red
  led_pattern->add_strip_state(red, off, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, red, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, red, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, red, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, red, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, red, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, red, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, off, red, 100);
  add_pattern(4, 1, led_pattern);

  led_pattern = new LedStripPattern((char*)"flying1");                      // left front out to in red
  led_pattern->add_strip_state(off, off, off, off, off, off, off, red, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, red, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, red, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, red, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, red, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, red, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, red, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(red, off, off, off, off, off, off, off, 100);
  add_pattern(4, 2, led_pattern);
  
  led_pattern = new LedStripPattern((char*)"flying1");                      // right rear in to out green
  led_pattern->add_strip_state(green, off, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, green, off, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, green, off, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, green, off, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, green, off, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, green, off, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, green, off, 100);
  led_pattern->add_strip_state(off, off, off, off, off, off, off, green, 100);
  add_pattern(4, 3, led_pattern);
}

LedBulbColor::LedBulbColor(uint8_t red_param, uint8_t green_param, uint8_t blue_param) {
  red = red_param;
  green = green_param;
  blue = blue_param;
}

LedStripState::LedStripState() {
    state_time = LED_DEFAULT_STATE_TIME;
}

LedStripState::LedStripState(uint16_t time_param) {
  state_time = time_param;
}

LedStripState::LedStripState(LedBulbColor* c1, LedBulbColor* c2, LedBulbColor* c3, LedBulbColor* c4, LedBulbColor* c5, LedBulbColor* c6, LedBulbColor* c7, LedBulbColor* c8, uint16_t time_param) {
  int bulb_count = 0;
  bulbs[bulb_count++] = c1;
  bulbs[bulb_count++] = c2;
  bulbs[bulb_count++] = c3;
  bulbs[bulb_count++] = c4;
  bulbs[bulb_count++] = c5;
  bulbs[bulb_count++] = c6;
  bulbs[bulb_count++] = c7;
  bulbs[bulb_count++] = c8;
  state_time = time_param;
}

LedStripPattern::LedStripPattern(char* name_param) {
  pattern_name = name_param;
  strip_state_count = 0;
}

void LedStripPattern::add_strip_state(LedStripState* strip_state_param) {
  if(strip_state_count < LED_MAX_STRIP_STATES) {
    led_strip_states[strip_state_count] = strip_state_param;
    strip_state_count++;  
  }
}

void LedStripPattern::add_strip_state(LedBulbColor* c1, LedBulbColor* c2, LedBulbColor* c3, LedBulbColor* c4, LedBulbColor* c5, LedBulbColor* c6, LedBulbColor* c7, LedBulbColor* c8, uint16_t time_param) {
  LedStripState* strip_state = new LedStripState(c1, c2, c3, c4, c5, c6, c7, c8, time_param);
  add_strip_state(strip_state);
}

LedStrip::LedStrip() {
  for(int i=0; i<LED_MAX_PATTERNS_PER_STRIP; i++) {
    led_patterns[i] = NULL;
  }
}

void LedController::add_pattern(int pattern_number, int strip_number, LedStripPattern* pattern) {
  if(pattern_number < LED_MAX_PATTERNS_PER_STRIP) {
    LedStrip* strip_ptr = led_strips[strip_number];
    strip_ptr->led_patterns[pattern_number] = pattern;
  }
}

void LedController::change_led_state(int strip_number, LedStripPattern* pattern, LedStrip* strip_ptr) {
  LedStripState* strip_state = pattern->led_strip_states[strip_ptr->current_state];
  strip_ptr->current_state_expiry_time = millis() + strip_state->state_time;
  for(uint8_t j=0; j<8; j++) {
    LedBulbColor* bulb_state = strip_state->bulbs[j];
    leds->setPixel(j+(strip_number*8), bulb_state->red << 16 | bulb_state->green << 8 | bulb_state->blue);
  }     
}

void LedController::process_10_millisecond() {
  static int8_t previous_pattern_index = -1;
  int8_t pattern_index = 0;
  uint8_t reverse = 0;
  
  if(mav->rc8 < 1050) {
    pattern_index = 0;
  } else if(mav->rc8 >= 1050 && mav->rc8 < 1150) {                             
    pattern_index = 1;
  } else if(mav->rc8 >= 1150 && mav->rc8 < 1250) {
    pattern_index = 2;
  } else if(mav->rc8 >= 1250 && mav->rc8 < 1350) {
    pattern_index = 3;
    if(mav->climb_rate < 0) {
      reverse = 1;
    }
  } else if(mav->rc8 >= 1350) {
    pattern_index = 4;
  }

//pattern_index = 3;
//reverse = 0;

  for(int i=0; i<8; i++) {
    LedStrip* strip_ptr = led_strips[i];
    LedStripPattern* pattern = strip_ptr->led_patterns[pattern_index]; 
    if(pattern != NULL) {  
      if(previous_pattern_index != pattern_index) {
        strip_ptr->current_state = 0;
        change_led_state(i, pattern, strip_ptr);
        previous_pattern_index = pattern_index;
      } else {
        if(millis() >= strip_ptr->current_state_expiry_time) { 
          if(reverse)
          {
            strip_ptr->current_state = ((strip_ptr->current_state - 1 + pattern->strip_state_count) % pattern->strip_state_count);
          } else {
            strip_ptr->current_state = ((strip_ptr->current_state + 1) % pattern->strip_state_count);
          }         
          change_led_state(i, pattern, strip_ptr);
        }
      }
    }
  }
  leds->show();
}

