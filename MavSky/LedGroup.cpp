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

extern MavConsole *console;

LedGroups::LedGroups(OctoWS2811* led_ptr) {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    group_ptrs[i] = new LedGroup(led_ptr, this, i);
  }
}

void LedGroups::dump_diags() {
  for(int i=0; i<led_layer_count; i++) {
    uint16_t layer_key = layers[i];
    uint8_t group_number = layer_key >> 8;
    uint8_t action_number = layer_key & 0xff;
    LedGroup* group_ptr = group_ptrs[group_number];

    LedGroupActions* led_group_actions = group_ptr->group_actions_ptr;
    LedGroupAction* led_group_action = led_group_actions->group_action_ptrs[action_number];
    
    uint8_t enabled = led_group_action->enabled;
    console->console_print("layer:%d - group:%d action:%d enabled:%d\r\n", i, group_number, action_number, enabled);
  }
//  for(int i=0; i<MAX_LED_GROUPS; i++) {
//    group_ptrs[i]->dump_diags();
//  }  
}

LedGroup* LedGroups::get_led_group(uint8_t group_number) {
  if(group_number < MAX_LED_GROUPS) {
    return group_ptrs[group_number];
  } else {
    return NULL;
  }
}

void LedGroups::clear_led_assignments() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->clear_led_assignments();
  }
}

void LedGroups::clear_all_actions() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->clear_all_actions();
  }
}

void LedGroups::disable_all_actions() {
  for(int i=0; i<MAX_LED_GROUPS; i++) {
    LedGroup* group_ptr = group_ptrs[i];
    group_ptr->group_actions_ptr->disable_all_actions();
  }
}

void LedGroups::process_10_milliseconds() {
  for(int i=led_layer_count-1; i>=0; i--) {
    uint16_t layer_key = layers[i];
    uint8_t group_number = layer_key >> 8;
    uint8_t action_number = layer_key & 0xff;
    if(group_number < MAX_LED_GROUPS && action_number < MAX_LED_ACTIONS_PER_GROUP) {
      LedGroup* led_group = group_ptrs[group_number];
      LedGroupActions* led_group_actions = led_group->group_actions_ptr;
      LedGroupAction* led_group_action = led_group_actions->group_action_ptrs[action_number];
      led_group_action->process_10_millisecond();     
    }
  }
}

void LedGroups::push_layer(uint8_t group_number, uint8_t action_number) {
  uint16_t layer_key = (group_number << 8) | (action_number & 0xff);
  uint8_t old_pos = MAX_LED_LAYERS;                           // default to after end
  for(int i=0; i<led_layer_count; i++) {
    if(layers[i] == layer_key) {  
      old_pos = i;
      break;
    }
  }
  if(old_pos == MAX_LED_LAYERS && led_layer_count < MAX_LED_LAYERS) {                             
    led_layer_count++;
  }
  for(int i=old_pos; i>0; i--) {
    if(i < MAX_LED_LAYERS) {
      layers[i] = layers[i-1];
    }
  }
  layers[0] = layer_key;
}

///////////////////////////////////////////////////////////////////////////////////////////

LedGroup::LedGroup(OctoWS2811* led_ptr, LedGroups* led_groups_param, int group_number_param) {
  led_groups_ptr = led_groups_param;
  group_number = group_number_param;
  group_actions_ptr = new LedGroupActions(led_ptr, this);
  for(int i=0; i<MAX_LEDS_PER_GROUP; i++) {
    strip_number[i] = -1;
    led_position[i] = -1;
    led_count = 0;
  } 
}

void LedGroup::dump_diags() {
  group_actions_ptr->dump_diags();
}

void LedGroup::clear_led_assignments() {
  for(int i=0; i<MAX_LEDS_PER_GROUP; i++) {
    strip_number[i] = -1;
    led_position[i] = -1;
    led_count = 0;
  }  
}

void LedGroup::clear_all_actions() {
  group_actions_ptr->clear_all_actions(); 
}

void LedGroup::push_layer(uint8_t action_number) {
  led_groups_ptr->push_layer(group_number, action_number);
}



