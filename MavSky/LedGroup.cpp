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
    group_ptrs[i] = new LedGroup(led_ptr, this, i);
    layers[i] = i;
  }
}

LedGroup* LedGroups::get_led_group(uint8_t group_number) {
  if(group_number < MAX_LED_GROUPS) {
    return group_ptrs[group_number];
  } else {
    return NULL;
  }
}

void LedGroups::push_layer(uint8_t group_number) {              // puts the most recent layer at the top so it will get processed first
  uint8_t old_group_pos = 0;
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    if(layers[i] == group_number) {
      old_group_pos = i;
      break;
    }
  }
  for(int i=old_group_pos; i>0; i--) {
    layers[i] = layers[i-1];
  }
  layers[0] = group_number;
}

void LedGroups::clear_all() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->clear();
  }
}

void LedGroups::disable_all() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->disable();
  }
}

void LedGroups::process_10_milliseconds() {
  for(int i=led_group_count-1; i>=0; i--) {
    int group = layers[i];
    LedGroup* group_pnt = get_led_group(group);
    group_pnt->process_10_millisecond();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

LedGroup::LedGroup(OctoWS2811* led_ptr, LedGroups* groups_ptr, uint8_t group_number_param) {
  leds = led_ptr;
  led_groups_ptr = groups_ptr;
  group_number = group_number_param;
  for(int i=0; i<MAX_LEDS_PER_GROUP; i++) {
    strip_number[i] = -1;
    led_position[i] = -1;
    led_count = 0;
  } 
}

void LedGroup::push_layer() {
  led_groups_ptr->push_layer(group_number);
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
  on_time = 0;                    
  off_time = 0;                     
  state_time = 0;  
  on_width = 0;                        
  on_led_count = 0;                      
  intensity = 0;                         
  reverse = 0;                            
  offset_time = 0;                            
  led_count = 0; 
}

void LedGroup::disable() {                            
  enabled = 0; 
}

void LedGroup::set_solid(uint32_t color) {
  push_layer();
  enabled = 1;
  if(mode != GROUP_MODE_SOLID || on_color != color) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_SOLID;
    on_color = color;
    on_time = 0;
    off_time = 0;
  }  
}

void LedGroup::set_dormant() {
  if(mode != GROUP_MODE_DORMANT) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_DORMANT;
    on_color = 0;
    on_time = 0;
    off_time = 0;
  }  
}

void LedGroup::set_flash(uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param) {
  push_layer();
  enabled = 1;
  if(mode != GROUP_MODE_FLASH || on_color != on_color_param || on_time != on_time_param || off_time != off_time_param || offset_time != offset_time_param) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_FLASH;
    on_color = on_color_param;
    on_time = on_time_param;
    off_time = off_time_param;
    offset_time = offset_time_param;
  }
}

void LedGroup::set_wave(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param, uint8_t reverse_param) {
  push_layer();
  enabled = 1;
  if(mode != GROUP_MODE_WAVE || reverse != reverse_param || on_color != on_color_param || state_time != state_time_param || on_width != on_width_param) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_WAVE;
    on_color = on_color_param;
    state_time = state_time_param;
    on_width = on_width_param;
    reverse = reverse_param;
  }  
}

void LedGroup::set_bounce(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param) {
  push_layer();
  enabled = 1;
  if(mode != GROUP_MODE_BOUNCE || on_color != on_color_param || state_time != state_time_param || on_width != on_width_param) {     
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_BOUNCE;
    on_color = on_color_param;
    state_time = state_time_param;
    on_width = on_width_param;
    reverse = 0;
  }  
}

void LedGroup::set_random(uint32_t state_time_param, uint8_t intensity_param) {
  push_layer();
  enabled = 1;
  state_time = state_time_param;
  if(mode != GROUP_MODE_RANDOM || intensity != intensity_param) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_RANDOM;
    on_color = 0;
    on_width = 0;
    intensity = intensity_param;
  }  
}

void LedGroup::set_bar(uint32_t on_color_param, uint32_t percent_param, uint8_t reverse_param) {
  push_layer();
  enabled = 1;
  if(mode != GROUP_MODE_BAR || reverse != reverse_param || on_color != on_color_param || percent_param != percent_param) {
    sequence_ms = 0;
    state = 0;
    mode = GROUP_MODE_BAR;
    on_color = on_color_param;
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

  if(!enabled) {
    return;
  }
  switch(mode) {
    case GROUP_MODE_SOLID:
      for(int i = 0; i<led_count; i++) {      
        uint8_t strip = strip_number[i];
        uint8_t pos = led_position[i];
        if(strip >= 0) {
          leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
        }
      }
      break;  

    case GROUP_MODE_DORMANT:
      break;  
            
    case GROUP_MODE_FLASH:
      {
        if((sequence_ms >= offset_time) && (sequence_ms < (offset_time + on_time))) {
          for(int i = 0; i<led_count; i++) {      
            uint8_t strip = strip_number[i];
            uint8_t pos = led_position[i];
            if(strip >= 0) {
              leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
            }
          }
        }
        sequence_ms += 10;   
        if(sequence_ms > (on_time + off_time)) {
          sequence_ms = 0;    
        }
      }
      break;

    case GROUP_MODE_WAVE:
      {
        for(int i=state; i<state+on_width; i++) {
          uint8_t on_led = i % led_count;
          uint8_t strip = strip_number[on_led];
          if(strip >= 0) {
            uint8_t pos = led_position[on_led];
            if(reverse) {
              pos = led_count - pos - 1;
            }          
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }
        if(sequence_ms >= state_time) {
          state = (state + 1) % led_count;
          sequence_ms = 0;       
        } else {
          sequence_ms += 10;       
        }
      
      }
      break;   
       
    case GROUP_MODE_BOUNCE:
      {
        num_states = led_count - on_width + 1;
        for(int on_led=state; on_led<state+on_width; on_led++) {
          uint8_t strip = strip_number[on_led];
          if(strip >= 0) {
            uint8_t pos = led_position[on_led];
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }     
        if(sequence_ms >= state_time) {
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
      }
      break;    

    case GROUP_MODE_RANDOM:
      {
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
      }
      break;

    case GROUP_MODE_BAR:
      {
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
      }
      break;
           
    default:
      break;
  }
}
