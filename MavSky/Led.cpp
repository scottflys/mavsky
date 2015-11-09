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
  sequence_complete = 0;
}

void Led::sequence_all_off() {
  if(!sequence_complete) {
    for(uint8_t i=0; i<leds_on_strip; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
    sequence_complete = 1;
  }  
}

void Led::sequence_normal() {
  if(!sequence_complete) {
    for(uint8_t i=0; i<leds_on_strip; i++) {
      if(i == 0) {
        strip.setPixelColor(i, 0, 255, 0);
      } else if(i == (leds_on_strip-1)) {
        strip.setPixelColor(i, 255, 0, 0);
      } else {
        strip.setPixelColor(i, 0, 0, 0);
      }
    }
    strip.show();
    sequence_complete = 1;
  }  
}

void Led::sequence_landing1() {
  if(!sequence_complete) {
    for(uint8_t i=0; i<leds_on_strip; i++) {
      if(i == 0) {
        strip.setPixelColor(i, 0, 255, 0);
      } else if(i == (leds_on_strip-1)) {
        strip.setPixelColor(i, 255, 0, 0);
      } else {
        strip.setPixelColor(i, 255, 255, 255);
      }
    }
    strip.show();
    sequence_complete = 1;
  }  
}

void Led::sequence_landing2(uint8_t descending) {
  // 0                    this are descending state sequences
  // 1 R             G    
  // 2 R-W         W-G
  // 3 R-W-W-    W-W-G
  // 4 R-W-W-W-W-W-W-G
  state_count = 5;
  switch(state) {
    case 0:
      for(uint8_t i=0; i<leds_on_strip; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      break;
    case 1:
      strip.setPixelColor(0, 0, 255, 0);
      for(uint8_t i=1; i<leds_on_strip-1; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.setPixelColor(leds_on_strip-1, 255, 0, 0);
      break;
    case 2:
      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 255, 255, 255);
      for(uint8_t i=2; i<leds_on_strip-2; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.setPixelColor(leds_on_strip-2, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-1, 255, 0, 0);
      break;
    case 3:
      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 255, 255, 255);
      strip.setPixelColor(2, 255, 255, 255);
      for(uint8_t i=3; i<leds_on_strip-3; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.setPixelColor(leds_on_strip-3, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-2, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-1, 255, 0, 0);
      break;  
    case 4:
      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 255, 255, 255);
      strip.setPixelColor(2, 255, 255, 255);
      strip.setPixelColor(3, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-4, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-3, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-2, 255, 255, 255);
      strip.setPixelColor(leds_on_strip-1, 255, 0, 0);
      break;  
  }
  strip.show();
  if(descending) {
    state = ((state + 1) % state_count);
  } else {
    state = ((state + state_count - 1) % state_count);
  }
}

uint8_t previous_pattern = 0;

void Led::process_200_millisecond() {
  uint8_t pattern = 0;

  if(mav->rc8 < 1050) {
    pattern = 1;
  } else if(mav->rc8 >= 1050 && mav->rc8 < 1150) {                             
    pattern = 2;
  } else if(mav->rc8 >= 1150 && mav->rc8 < 1250) {
    pattern = 3;
  } else if(mav->rc8 >= 1250) {
    pattern = 4;
  }
  
  if(pattern != previous_pattern) {
    sequence_complete = 0;
    state = 0;
    previous_pattern = pattern;
  }
  switch(pattern) {
    case 1:
      sequence_all_off();
      break; 
    case 2:
      sequence_normal();
      break; 
    case 3:
      sequence_landing1();
      break;     
    case 4:
      sequence_landing2(mav->climb_rate < 0);
      break;          
  }

}
  

