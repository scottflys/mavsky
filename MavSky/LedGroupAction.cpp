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
#include "LedGroupAction.h"
#include "LedGroup.h"
#include "Led.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;
extern int displayMemory[];
extern int drawingMemory[];

LedGroupActions::LedGroupActions(OctoWS2811* led_ptr, LedGroup* led_group_ptr_param) {
  led_group_ptr = led_group_ptr_param;
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    group_action_ptrs[i] = new LedGroupAction(led_ptr, led_group_ptr_param);
  }
}

void LedGroupActions::push_layer(uint8_t action_index_number) {  
  led_group_ptr->push_layer(action_index_number);     
}


void LedGroupActions::disable_all_actions() {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* group_action_ptr = group_action_ptrs[i];
    group_action_ptr->disable();
  }
}


int LedGroupActions::get_next_empty_action() {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* existing_action = group_action_ptrs[i];
    if(existing_action->in_use == 0) {
      existing_action->in_use = 1; 
      return i;
    }
  }
  return -1;
}

void LedGroupActions::set_solid(uint32_t color_param) {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_SOLID && action->on_color == color_param) {
      action->enabled = 1;
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_SOLID, color_param, 0, 0, 0, 0, 0, 0, 0, 0);    
    push_layer(i);    
  } 
}

void LedGroupActions::set_flash(uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param) {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_FLASH && action->on_color == on_color_param && action->on_time == on_time_param && action->off_time == off_time_param && action->offset_time == offset_time_param) {
      action->enabled = 1;
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_FLASH, on_color_param, on_time_param, off_time_param, offset_time_param, 0, 0, 0, 0, 0);    
    push_layer(i);    
  } 
}

void LedGroupActions::set_wave(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param, uint8_t reverse_param) {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_WAVE && action->on_color == on_color_param && action->state_time == state_time_param && action->on_width == on_width_param && action->reverse == reverse_param) {
      action->enabled = 1;
      
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_WAVE, on_color_param, 0, 0, 0, state_time_param, on_width_param, 0, 0, reverse_param);    
    push_layer(i);    
  } 
}

void LedGroupActions::set_bounce(uint32_t on_color_param, uint32_t state_time_param, uint32_t on_width_param) {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_BOUNCE && action->on_color == on_color_param && action->state_time == state_time_param && action->on_width == on_width_param) {
      action->enabled = 1;
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_BOUNCE, on_color_param, 0, 0, 0, state_time_param, on_width_param, 0, 0, 0);    
    push_layer(i);    
  }   
}

void LedGroupActions::set_random(uint32_t state_time_param, uint8_t intensity_param) {
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_RANDOM && action->state_time == state_time_param && action->intensity == intensity_param) {
      action->enabled = 1;
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_RANDOM, 0, 0, 0, 0, state_time_param, 0, intensity_param, 0, 0);    
    push_layer(i);    
  }
}

void LedGroupActions::set_bar(uint32_t on_color_param, uint32_t percent_param, uint8_t reverse_param) {
  uint8_t on_led_count = ((led_group_ptr->led_count * percent_param) + (led_group_ptr->led_count / 2)) / 100;
  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
    LedGroupAction* action = group_action_ptrs[i];
    if(action->mode == GROUP_MODE_BAR && action->on_color == on_color_param && action->on_led_count == on_led_count && action->reverse == reverse_param) {
      action->enabled = 1;
      push_layer(i);
      return;
    }
  }
  int i = get_next_empty_action();
  if(i >= 0) {
    LedGroupAction* action = group_action_ptrs[i];
    action->init(GROUP_MODE_BAR, on_color_param, 0, 0, 0, 0, 0, 0, on_led_count, reverse_param);    
    push_layer(i);    
  }
  

  
//  push_layer();
//  enabled = 1;
//  if(mode != GROUP_MODE_BAR || reverse != reverse_param || on_color != on_color_param || percent_param != percent_param) {
//    sequence_ms = 0;
//    state = 0;
//    mode = GROUP_MODE_BAR;
//    on_color = on_color_param;
//    on_time = 0;
//    off_time = 0;
//    on_led_count = (led_count * (percent_param + 5)) / 100;
//    reverse = reverse_param;
//  }  
}

//void LedGroupActions::process_10_milliseconds() {
//  for(int i=0; i<MAX_LED_ACTIONS_PER_GROUP; i++) {
//    LedGroupAction* group_action_ptr = group_action_ptrs[i];
//    group_action_ptr->process_10_millisecond();
//  }
//}





///////////////////////////////////////////////////////////////////////////////////////////////////////////

LedGroupAction::LedGroupAction(OctoWS2811* led_ptr, LedGroup* group_ptr_param) {
  leds = led_ptr;
  group_ptr = group_ptr_param;
}

//    uint8_t enabled = 0;
//    uint8_t in_use = 0;
//    uint32_t sequence_ms = 0;
//    uint8_t  state = 0;
//    uint8_t  mode = 0;    
//    uint32_t on_color = 0;
//    uint32_t on_time = 0;                             // flash
//    uint32_t off_time = 0;                            // flash
//    uint32_t state_time = 0;  
//    uint8_t  on_width = 0;                            // wave
//    uint8_t  on_led_count = 0;                        // bar
//    uint8_t  intensity = 0;                           // random
//    uint8_t  reverse = 0;                             // wave, bar
//    uint32_t offset_time = 0;                             // flash2


void LedGroupAction::init(uint8_t mode_param, uint32_t on_color_param, uint32_t on_time_param, uint32_t off_time_param, uint32_t offset_time_param, uint32_t state_time_param, uint32_t on_width_param, uint8_t intensity_param, uint8_t on_led_count_param, uint8_t reverse_param) {
  //console->console_print("Action::init mode:%d on_color:%06x on_time:%d off_time:%d offset_time:%d state_time:%d on_width:%d intensity:%d on_led_count:%d reverse:%d\r\n", mode_param, on_color_param, on_time_param, off_time_param, offset_time_param, state_time_param, on_width_param, intensity_param, on_led_count_param, reverse_param);
  mode = mode_param;
  on_color = on_color_param;
  on_time = on_time_param;
  off_time = off_time_param;
  offset_time = offset_time_param;
  state_time = state_time_param;
  on_width = on_width_param;
  intensity = intensity_param;
  on_led_count = on_led_count_param;
  reverse = reverse_param;
  sequence_ms = 0;
  state = 0;
  in_use = 1;
  enabled = 1;
}

void LedGroupAction::disable() {                            
  enabled = 0; 
}

void LedGroupAction::force_off() {
  for(int i = 0; i<group_ptr->led_count; i++) {      
    uint8_t strip = group_ptr->strip_number[i];
    uint8_t pos = group_ptr->led_position[i];
    if(strip >= 0) {
      leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), 0);
    }     
  }   
}

void LedGroupAction::process_10_millisecond() {
  uint8_t num_states = 0; 
  if(!enabled) {
    return;
  }
  switch(mode) {
    case GROUP_MODE_SOLID:    
      for(int i = 0; i<group_ptr->led_count; i++) {      
        uint8_t strip = group_ptr->strip_number[i];
        uint8_t pos = group_ptr->led_position[i];
        if(strip >= 0) {
          leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
        }
      }
      break;  

    case GROUP_MODE_FLASH:
      {
        if((sequence_ms >= offset_time) && (sequence_ms < (offset_time + on_time))) {
          for(int i = 0; i<group_ptr->led_count; i++) { 
            uint8_t strip = group_ptr->strip_number[i];
            uint8_t pos = group_ptr->led_position[i];
            if(strip >= 0) {
              leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
            }
          }
        }
        sequence_ms += 10;   
        if(sequence_ms >= (on_time + off_time)) {
          sequence_ms = 0;    
        }
      }
      break;

    case GROUP_MODE_WAVE:
      {        
        for(int i=state; i<state+on_width; i++) {
          uint8_t on_led = i % group_ptr->led_count;
          uint8_t strip = group_ptr->strip_number[on_led];
          if(strip >= 0) {
            uint8_t pos = group_ptr->led_position[on_led];
            if(reverse) {
              pos = group_ptr->led_count - pos - 1;
            }          
            leds->setPixel(pos+(strip*MAX_LEDS_PER_STRIP), on_color);
          }
        }
        if(sequence_ms >= state_time) {
          state = (state + 1) % group_ptr->led_count;
          sequence_ms = 0;       
        } else {
          sequence_ms += 10;       
        }    
      }
      break;   
       
    case GROUP_MODE_BOUNCE:
      {
        num_states = group_ptr->led_count - on_width + 1;
        for(int on_led=state; on_led<state+on_width; on_led++) {
          uint8_t strip = group_ptr->strip_number[on_led];
          if(strip >= 0) {
            uint8_t pos = group_ptr->led_position[on_led];
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
          for(int i = 0; i<group_ptr->led_count; i++) {      
            uint32_t color = (random(0, intensity) << 16) + (random(0, intensity) << 8) + random(0, intensity);
            uint8_t strip = group_ptr->strip_number[i];
            uint8_t pos = group_ptr->led_position[i];
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
          uint8_t strip = group_ptr->strip_number[i];
          if(strip >= 0) {
            uint8_t pos = group_ptr->led_position[i];
            if(reverse) {
              pos = group_ptr->led_count - pos - 1;
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
