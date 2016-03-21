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

#include "MavSky.h"
#include "OctoWS2811.h"
#include "LedGroup.h"
#include "Led.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;
extern int displayMemory[];
extern int drawingMemory[];

LedGroups::LedGroups(OctoWS2811* led_ptr) {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    group_ptrs[i] = new LedGroup(led_ptr);
  }
}

LedGroup* LedGroups::get_led_group(uint8_t group_number) {
  if(group_number < MAX_LED_GROUPS) {
    return group_ptrs[group_number];
  } else {
    return NULL;
  }
}

void LedGroups::clear_all() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->clear();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

LedGroup::LedGroup(OctoWS2811* led_ptr) {
  leds = led_ptr;
  for(int i=0; i<MAX_LEDS_PER_GROUP; i++) {
    strip_number[i] = -1;
    led_position[i] = -1;
    led_count = 0;
  }  
}

void LedGroup::clear() {
  force_off();
  for(int i=0; i<MAX_LEDS_PER_GROUP; i++) {
    strip_number[i] = -1;
    led_position[i] = -1;
    led_count = 0;
  }  
  sequence_ms = 0;
  state = 0;
  mode = 0;    
  on_color = 0;
  off_color = 0;
  on_time = 0;                    
  off_time = 0;                     
  state_time = 0;  
  on_width = 0;                        
  on_led_count = 0;                      
  intensity = 0;                         
  reverse = 0;                            
  led_count = 0; 
}

void LedGroup::set_solid(uint32_t color) {
  if(mode != GROUP_MODE_SOLID) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_SOLID;
    on_color = color;
    off_color = 0;
    on_time = 0;
    off_time = 0;
  }  
}

void LedGroup::set_flash(uint32_t on_color_param, uint32_t off_color_param, uint32_t on_time_param, uint32_t off_time_param) {
  if(mode != GROUP_MODE_FLASH) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_FLASH;
    on_color = on_color_param;
    off_color = off_color_param;
    on_time = on_time_param;
    off_time = off_time_param;
  }  
}

void LedGroup::set_wave(uint8_t reverse_param, uint32_t on_color_param, uint32_t off_color_param, uint32_t state_time_param, uint32_t on_width_param) {
  if(mode != GROUP_MODE_WAVE) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_WAVE;
    on_color = on_color_param;
    off_color = off_color_param;
    state_time = state_time_param;
    on_width = on_width_param;
    reverse = reverse_param;
  }  
}

void LedGroup::set_bounce(uint8_t reverse_param, uint32_t on_color_param, uint32_t off_color_param, uint32_t state_time_param, uint32_t on_width_param) {
  if(mode != GROUP_MODE_BOUNCE) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_BOUNCE;
    on_color = on_color_param;
    off_color = off_color_param;
    state_time = state_time_param;
    on_width = on_width_param;
    reverse = reverse_param;
  }  
}

void LedGroup::set_random(uint32_t state_time_param, uint8_t intensity_param) {
  state_time = state_time_param;
  if(mode != GROUP_MODE_RANDOM) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_RANDOM;
    on_color = 0;
    off_color = 0;
    on_width = 0;
    intensity = intensity_param;
  }  
}

void LedGroup::set_bar(uint8_t reverse_param, uint32_t on_color_param, uint32_t off_color_param, uint32_t percent_param) {
  if(mode != GROUP_MODE_BAR) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_BAR;
    on_color = on_color_param;
    off_color = off_color_param;
    on_time = 0;
    off_time = 0;
    on_led_count = (led_count * (percent_param + 5)) / 100;
    reverse = reverse_param;
  }  
}

void LedGroup::force_off() {
  for(int i = 0; i<led_count; i++) {      
    uint8_t strip = strip_number[i];
    uint8_t pos = led_position[i];
    if(strip >= 0) {
      leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), 0);
    }     
  }   
}

void LedGroup::process_10_millisecond() {
  uint8_t num_states = 0;
  
  switch(mode) {
    case GROUP_MODE_SOLID:
      if(state != 1) {
        for(int i = 0; i<led_count; i++) {      
          uint8_t strip = strip_number[i];
          uint8_t pos = led_position[i];
          if(strip >= 0) {
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }
        state = 1;
      }
      break;
      
    case GROUP_MODE_FLASH:
      if(sequence_ms < on_time) {
        if(state != 1) {
          for(int i = 0; i<led_count; i++) {      
            uint8_t strip = strip_number[i];
            uint8_t pos = led_position[i];
            if(strip >= 0) {
              leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
            }
          }
          state = 1;
        }
        sequence_ms += 10;     
      } else if (sequence_ms < (on_time + off_time)) {
        if(state != 0) {
          for(int i = 0; i<led_count; i++) {      
            uint8_t strip = strip_number[i];
            uint8_t pos = led_position[i];
            if(strip >= 0) {
              leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), off_color);
            }
          }
          state = 0;
        }
        sequence_ms += 10;     
      } else {
        sequence_ms = 0;    
      }
      break;
      
    case GROUP_MODE_WAVE:
      if(sequence_ms >= state_time) {
        uint8_t on_led = state;
        uint8_t off_led = (state + led_count - on_width) % led_count;
        
        uint8_t strip = strip_number[on_led];
        if(strip >= 0) {
          uint8_t pos = led_position[on_led];
          if(reverse) {
            pos = led_count - pos - 1;
          }          
          leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
        }
        
        strip = strip_number[off_led];
        if(strip >= 0) {
          uint8_t pos = led_position[off_led];
          if(reverse) {
            pos = led_count - pos - 1;
          } 
          leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), off_color);
        }

        state = (state + 1) % led_count;
        sequence_ms = 0;       
      } else {
        sequence_ms += 10;       
      }
      break;    
      
    case GROUP_MODE_BOUNCE:
      if(sequence_ms >= state_time) {
        num_states = led_count - on_width + 1;
//console->console_print("\r\ns:%d ", state);
        for(int off_led=0; off_led<state; off_led++) {
//console->console_print("a:%d ", off_led);
          uint8_t strip = strip_number[off_led];
          if(strip >= 0) {
            uint8_t pos = led_position[off_led];
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), off_color);
          }
        }
        for(int on_led=state; on_led<state+on_width; on_led++) {
//console->console_print("b:%d ", on_led);
          uint8_t strip = strip_number[on_led];
          if(strip >= 0) {
            uint8_t pos = led_position[on_led];
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }
        for(int off_led=state+on_width; off_led<led_count; off_led++) {
//console->console_print("c:%d ", off_led);
          uint8_t strip = strip_number[off_led];
          if(strip >= 0) {
            uint8_t pos = led_position[off_led];
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), off_color);
          }
        }        
        if(!reverse) {
          if(state+1 >= num_states) {
            reverse = 1;
            state--;
          } else {
            state++;
          }
        } else {
          if(state == 0) {
            state++;
            reverse = 0;
          } else {
            state--;
          }
        }
        sequence_ms = 0;               
      } else {
        sequence_ms += 10;       
      }
      break;    
      
    case GROUP_MODE_RANDOM:
      if(sequence_ms >= state_time) {
        for(int i = 0; i<led_count; i++) {      
          uint32_t color = (random(0, intensity) << 16) + (random(0, intensity) << 8) + random(0, intensity);
          uint8_t strip = strip_number[i];
          uint8_t pos = led_position[i];
          if(strip >= 0) {
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), color);
          }
        }
        sequence_ms = 0;       
      } else {
        sequence_ms += 10;       
      }
      break;

    case GROUP_MODE_BAR:
      if(state != 1) {
        for(int i = 0; i < on_led_count; i++) {      
          uint8_t strip = strip_number[i];
          if(strip >= 0) {
            uint8_t pos = led_position[i];
            if(reverse) {
              pos = led_count - pos - 1;
            }
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }
        for(int i = on_led_count; i<led_count; i++) {      
          uint8_t strip = strip_number[i];
          if(strip >= 0) {
            uint8_t pos = led_position[i];
            if(reverse) {
              pos = led_count - pos - 1;
            }
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), off_color);
          }
        }
        state = 1;
      }
      break;
            
    default:
      break;
  }
}
