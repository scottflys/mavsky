#include "Led.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;

Led::Led(uint8_t pin, uint8_t led_count) {
  leds_on_strip = led_count;
  strip = Adafruit_NeoPixel(60, pin, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.show();                                     // all pixels off
 
  LedPattern* led_pattern = new LedPattern((char*)"all_off"); 
  LedStripState* strip_state = new LedStripState();
  strip_state->add_bulbs(0, 0, 0, 8);
  led_pattern->add_strip_state(strip_state);
  add_pattern(led_pattern);

  led_pattern = new LedPattern((char*)"normal"); 
  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulbs(0, 0, 0, 6);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state);
  add_pattern(led_pattern);

  led_pattern = new LedPattern((char*)"landing1"); 
  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulbs(255, 255, 255, 6);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state); 
  add_pattern(led_pattern);
  
  // 0                    this are descending state sequences
  // 1 R             G    
  // 2 R-W         W-G
  // 3 R-W-W-    W-W-G
  // 4 R-W-W-W-W-W-W-G
  led_pattern = new LedPattern((char*)"landing2"); 
  strip_state = new LedStripState();
  strip_state->add_bulbs(0, 0, 0, 8);
  led_pattern->add_strip_state(strip_state);  

  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulbs(0, 0, 0, 6);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state);  

  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulb(255, 255, 255);
  strip_state->add_bulbs(0, 0, 0, 4);
  strip_state->add_bulb(255, 255, 255);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state);  

  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulbs(255, 255, 255, 2);
  strip_state->add_bulbs(0, 0, 0, 2);
  strip_state->add_bulbs(255, 255, 255, 2);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state);
    
  strip_state = new LedStripState();
  strip_state->add_bulb(255, 0, 0);
  strip_state->add_bulbs(255, 255, 255, 6);
  strip_state->add_bulb(0, 255, 0);
  led_pattern->add_strip_state(strip_state);  
  add_pattern(led_pattern);
   
}

LedBulbState::LedBulbState(uint8_t red_param, uint8_t green_param, uint8_t blue_param) {
  red = red_param;
  green = green_param;
  blue = blue_param;
}

LedStripState::LedStripState() {
  
}

void LedStripState::add_bulb(uint8_t red_param, uint8_t green_param, uint8_t blue_param) {
  bulbs[bulb_count] = new LedBulbState(red_param, green_param, blue_param);
  bulb_count++;
}

void LedStripState::add_bulbs(uint8_t red_param, uint8_t green_param, uint8_t blue_param, uint8_t count) {
  for(uint8_t i=0; i<count; i++) {
    add_bulb(red_param, green_param, blue_param);
  }
}

LedPattern::LedPattern(char* name_param) {
  pattern_name = name_param;
  strip_state_count = 0;
}

void LedPattern::add_strip_state(LedStripState* strip_state_param) {
  led_strip_states[strip_state_count] = strip_state_param;
  strip_state_count++;  
}

void Led::add_pattern(LedPattern* pattern) {
  led_patterns[led_pattern_count] = pattern;
  led_pattern_count++;
}

void Led::show_pattern(LedPattern* pattern, uint8_t init_pattern) {
  static uint8_t current_state = 0;

  if(init_pattern) {
    current_state = 0;
  }
  if(init_pattern || pattern->strip_state_count > 1) {                                // no need to do anything unless there is > 1 state
    LedStripState* strip_state = pattern->led_strip_states[current_state];
    for(uint8_t i=0; i<strip_state->bulb_count; i++) {
      LedBulbState* bulb_state = strip_state->bulbs[i];
      strip.setPixelColor(i, bulb_state->red, bulb_state->green, bulb_state->blue);
    }
    strip.show();
    current_state = ((current_state + 1) % pattern->strip_state_count);
  }
}

void Led::process_200_millisecond() {
  int8_t pattern_index = 0;
  static int8_t previous_pattern_index = -1;
  
  if(mav->rc8 < 1050) {
    pattern_index = 0;
  } else if(mav->rc8 >= 1050 && mav->rc8 < 1150) {                             
    pattern_index = 1;
  } else if(mav->rc8 >= 1150 && mav->rc8 < 1250) {
    pattern_index = 2;
  } else if(mav->rc8 >= 1250) {
    pattern_index = 3;
  }
      
  if(pattern_index != previous_pattern_index) {
    previous_pattern_index = pattern_index;
  }
  
  show_pattern(led_patterns[pattern_index], pattern_index != previous_pattern_index);
}



  

