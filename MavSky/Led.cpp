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

#include "../EEPROM/EEPROM.h"

extern MavLinkData *mav;
extern MavConsole *console;
extern int displayMemory[];
extern int drawingMemory[];

#define MS_PER_TIMESLICE       10


#define VAR_MAV_RC_CH7                  0x01      // mav.rc.ch7
#define VAR_MAV_RC_CH8                  0x02      // mav.rc.ch8

#define VAR_MAV_BATTERY_CURRENT         0x10      // mav.battery.current
#define VAR_MAV_BATTERY_VOLTAGE         0x11      // mav.battery.voltage
#define VAR_MAV_BATTERY_REMAINING       0x12      // mav.battery.remaining
#define VAR_MAV_BATTERY_CONSUMED        0x13      // mav.battery.consumed

#define VAR_MAV_SAT_HDOP                0x20      // mav.sat.hdop
#define VAR_MAV_SAT_VISIBLE             0x21      // map.sat.visible

#define VAR_MAV_VEHICLE_BARALTITUDE     0x30      // mav.vehicle.baraltitude
#define VAR_MAV_VEHICLE_COG             0x31      // mav.vehicle.cog
#define VAR_MAV_VEHICLE_HEADING         0x32      // mav.vehicle.heading
#define VAR_MAV_VEHICLE_SPEED           0x33      // mav.vehicle.speed

#define VAR_MAV_FC_ARMED                0x40      // mav.fc.armed
#define VAR_MAV_FC_FLIGHTMODE           0x41      // mav.fc.flightmode


#define CMD_LOAD_REG_CONST      1                 // rr cccccccc                     rr = register number, cccccccc = constant value
#define CMD_LOAD_REG_MAV        2                 // rr mm                           rr = register number, mm = mav value
#define CMD_PAUSE               3                 // rr                              rr = register with milliseconds               
#define CMD_YIELD               4                 //                                         
#define CMD_JUMP_ABS            5                 // aaaa                            aaaa = absolute jump
#define CMD_MOVE_REG            8                 // sr tr                           sr = source register, tr = target register                                                                    // todo test
#define CMD_PUSH                9                 // sr                              sr = source register
#define CMD_POP                 10                // tr                              tr = target register
#define CMD_OR                  11                // 
#define CMD_AND                 12                // 

#define CMD_BZ_REL              13                // rrrr                            rrrr = relative jump
#define CMD_BNZ_REL             14                // rrrr                            rrrr = relative jump

#define CMD_0EQ1                16                // 0 = 0 == 1
#define CMD_0NE1                17                // 0 = 0 != 1
#define CMD_0LT1                18                // 0 = 0 < 1
#define CMD_0LE1                19                // 0 = 0 <= 1
#define CMD_0GT1                20                // 0 = 0 > 1
#define CMD_0GE1                21                // 0 = 0 >= 1

#define CMD_GROUP_SET           32                // gn gs ge ln ls le               gn = group number, gs = group start, ge = group end, ln = led strip number, ls = led start, le = led end  
#define CMD_GROUP_CLEAR         33                // gn                              gn = group number
#define CMD_CLEAR_GROUPS        34                //                              
#define CMD_DISABLE_GROUPS      35                // disables all groups                            

#define CMD_SETCOLOR            48                // gg                              gg = group number  (implied: R0 = color)
#define CMD_SETFLASH            49                // gg                              gg = group number, (implied: R0 = on color, R1 = on time, R2 = off time, R3 = offset time)
#define CMD_SETWAVE             50                // gg                              gg = group number, (implied: R0 = on color, R1 = state time, R2 = on width, R3 = reverse)
#define CMD_SETRANDOM           51                // gg                              gg = group number, (implied: R0 = change time register, R1 = intensity) 
#define CMD_SETBAR              52                // gg                              gg = group number, (implied: R0 = on color, R1 = value, R2 = low, R3 = high, R4 = reverse)
#define CMD_SETBOUNCE           53                // gg                              gg = group number, (R0 = on color, R1 = state time, R2 = on width)
#define CMD_SETOFF              54                // gg                              gg = group number
#define CMD_SETDORMANT          55                // gg                              gg = group number

#define CMD_LDAA8               64                // cc                              cc = short constant
#define CMD_LDAB8               65                // cc                              cc = short constant
#define CMD_LDAC8               66                // cc                              cc = short constant
#define CMD_LDAD8               67                // cc                              cc = short constant
#define CMD_LDAE8               68                // cc                              cc = short constant

#define CMD_LDAA16              72                // cccc                              cc = short constant
#define CMD_LDAB16              73                // cccc                              cc = short constant
#define CMD_LDAC16              74                // cccc                              cc = short constant
#define CMD_LDAD16              75                // cccc                              cc = short constant
#define CMD_LDAE16              76                // cccc                              cc = short constant


uint8_t program[EEPROM_LED_CODE_MAX_SIZE];
uint16_t program_size = 0;

uint8_t led_map[8];

LedGroups* led_groups;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
        
LedController::LedController() {
  leds = new OctoWS2811(MAX_LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
  leds->begin();

  reload();
  
  leds->show();

  led_groups = new LedGroups(leds);
}

void LedController::reload() {
  pc = 0;
  uint16_t n = (EEPROM.read(EEPROM_LED_CODE_SIZE) << 8) + EEPROM.read(EEPROM_LED_CODE_SIZE+1);
  
  if(n < EEPROM_LED_CODE_MAX_SIZE) {
    program_size = n;
    for(uint16_t i=0; i<program_size; i++) {
      program[i] = EEPROM.read(EEPROM_LED_CODE_BASE + i);
    }
  }  

  int sum = 0;
  for(uint16_t i=0; i<8; i++) {
    sum += EEPROM.read(EEPROM_LED_MAP_BASE + i);
  }

  if(sum != 28) {
    console->console_print("Resetting led map base\r\n");
    for(uint16_t i=0; i<8; i++) {
      EEPROM.write(EEPROM_LED_MAP_BASE + i, i);
    }     
  }
  for(int i=0; i<8; i++) {
    led_map[i] = EEPROM.read(EEPROM_LED_MAP_BASE + i);
  }

  for (int i=0; i < MAX_LEDS_PER_STRIP*MAX_STRIPS; i++) {
    leds->setPixel(i, 0x000000);
  }
}

uint32_t LedController::get_variable(uint16_t input) {
  switch(input) {
    
    case VAR_MAV_RC_CH7:
      return mav->rc7;
      break; 
         
    case VAR_MAV_RC_CH8:
      return mav->rc8;
      break;
      
    case VAR_MAV_BATTERY_CURRENT:
      return mav->average_battery_current;
      break;
      
    case VAR_MAV_BATTERY_VOLTAGE:
      return mav->average_battery_voltage;
      break;
      
    case VAR_MAV_BATTERY_REMAINING:
      return mav->battery_remaining;
      break;
      
    case VAR_MAV_BATTERY_CONSUMED:
      return mav->tenth_amp_per_millisecond_consumed;
      break;

    case VAR_MAV_SAT_HDOP:
      return mav->gps_hdop;
      break;
      
    case VAR_MAV_SAT_VISIBLE:
      return mav->gps_satellites_visible;
      break;
      
    case VAR_MAV_VEHICLE_BARALTITUDE:
      return mav->bar_altitude;
      break;
      
    case VAR_MAV_VEHICLE_COG:
      return mav->calced_cog;
      break;
      
    case VAR_MAV_VEHICLE_HEADING:
      return mav->heading;
      break;
      
    case VAR_MAV_VEHICLE_SPEED:
      return mav->gps_speed;
      break;

    case VAR_MAV_FC_ARMED:
      return mav->base_mode >> 7;
      break;
      
    case VAR_MAV_FC_FLIGHTMODE:
      return mav->custom_mode;
      break;
      
    default:
      return 0;
      break;
  }
}

void LedController::cmd_group_set() {
  uint8_t  group_number = program[pc++];
  uint8_t  group_led_start = program[pc++];
  uint8_t  group_led_end = program[pc++];
  uint8_t  virtual_strip_number = program[pc++];
  uint8_t  strip_led_start = program[pc++];
  uint8_t  strip_led_end = program[pc++];

  if(virtual_strip_number > 8) {
    return;
  }
  uint8_t strip_number = led_map[virtual_strip_number];

  if(group_number < MAX_LED_GROUPS) {                                 // todo - beef up this check
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    if(abs((int)group_led_end - (int)group_led_start) == abs((int)strip_led_end - (int)strip_led_start)) {
      int8_t strip_increment = 1;
      if(strip_led_end < strip_led_start) {
        strip_increment = -1;
      }
      int strip_i = strip_led_start;
      if(group_led_end > group_led_start) {
        for(int group_i = group_led_start; group_i <= group_led_end; group_i++) {
          if(group_i < MAX_LEDS_PER_GROUP) {
            group_ptr->strip_number[group_i] = strip_number;
            group_ptr->led_position[group_i] = strip_i;      
            if(group_i >= group_ptr->led_count) {
              group_ptr->led_count = group_i + 1;
            }
          }
          strip_i += strip_increment;
        }        
      } else {
        for(int group_i = group_led_start; group_i >= group_led_end; group_i--) {
          if(group_i < MAX_LEDS_PER_GROUP) {
            group_ptr->strip_number[group_i] = strip_number;
            group_ptr->led_position[group_i] = strip_i;
            if(group_i >= group_ptr->led_count) {
              group_ptr->led_count = group_i + 1;
            }
          }
          strip_i += strip_increment;
        }
      }
    }
  }
}

void LedController::cmd_group_clear() {
  uint8_t  group_number = program[pc++];

  if(group_number < MAX_LED_GROUPS) {                        
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->clear();
  }
}

void LedController::cmd_disable_groups() {
  led_groups->disable_all();
}

void LedController::cmd_clear_groups() {
  led_groups->clear_all();
}

void LedController::cmd_load_reg_const() {
  uint8_t register_number = program[pc++];
  uint32_t register_value = program[pc++] << 24;
  register_value |= program[pc++] << 16;
  register_value |= program[pc++] << 8;
  register_value |= program[pc++];
  if(register_number < MAX_REGISTERS) {
    registers[register_number] = register_value;
  }
}

void LedController::cmd_load_reg_mav() {
  uint8_t register_number = program[pc++]; 
  uint8_t  mav_variable = program[pc++];
  if(register_number < MAX_REGISTERS) {
    registers[register_number] = get_variable(mav_variable);
  }
}

void LedController::cmd_pause() {
  uint8_t register_number = program[pc++];
  if(register_number < MAX_REGISTERS) {
    pausing_time_left = registers[register_number];
  }
}

void LedController::cmd_yield() {
  pausing_time_left = MS_PER_TIMESLICE;
}

void LedController::cmd_jump_absolute() {
  uint16_t next_pc = program[pc++] << 8;                             
  pc = next_pc;
}

void LedController::cmd_bz_relative() {
  uint16_t pc_change = program[pc++] << 8;                               
  pc_change |= program[pc++];                            
  if(registers[0] == 0) {
    pc += (int16_t)pc_change;
  }
}

void LedController::cmd_bnz_relative() {
  uint16_t pc_change = program[pc++] << 8;                               
  pc_change |= program[pc++];                            
  if(registers[0] != 0) {
    pc += (int16_t)pc_change;
  }
}

void LedController::cmd_move_register() {
  uint8_t source_register = program[pc++];
  uint8_t target_register = program[pc++];
  if(source_register < MAX_REGISTERS && target_register < MAX_REGISTERS ) {
    registers[target_register] = registers[source_register];
  }
}

void LedController::cmd_set_color() {
  uint8_t group_number = program[pc++];;
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_solid(registers[0]);
  }  
}

void LedController::cmd_set_flash() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_flash(registers[0], registers[1], registers[2], registers[3]);  
  }
}

void LedController::cmd_set_wave() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_wave(registers[0], registers[1], registers[2], registers[3]);  
  }
}
  
void LedController::cmd_set_bounce() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_bounce(registers[0], registers[1], registers[2]);  
  }
}
    
void LedController::cmd_set_random() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_random(registers[0], (uint8_t)registers[1]);  
  }
}

void LedController::cmd_set_bar() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    uint32_t value = registers[1];
    uint32_t low = registers[2];
    uint32_t high = registers[3];
    uint32_t percent = ((value - low) * 100) / (high - low);
    group_ptr->set_bar(registers[0], percent, registers[4]);  
  }
}

void LedController::cmd_set_off() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_solid(0);
  }
}

void LedController::cmd_set_dormant() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_dormant();
  }
}

void LedController::cmd_0eq1() {
  if(registers[0] ==  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_0ne1() {
  if(registers[0] !=  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_0lt1() {
  if(registers[0] <  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_0le1() {
  if(registers[0] <=  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_0gt1() {
  if(registers[0] >  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_0ge1() {
  if(registers[0] >=  registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_or() {
  if(registers[0] || registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_and() {
  if(registers[0] && registers[1]) {
    registers[0] = 1;
  } else {
    registers[0] = 0;
  }
}

void LedController::cmd_push() {
  uint8_t register_number = program[pc++];
  if(register_number < MAX_REGISTERS && stack_size < MAX_STACK_SIZE) {
    stack[stack_size++] = registers[register_number];
  }
}

void LedController::cmd_pop() {
  uint8_t register_number = program[pc++];
  if(register_number < MAX_REGISTERS && stack_size > 0) {
    registers[register_number] = stack[--stack_size];
  }
}

void LedController::cmd_load_reg_8(uint8_t reg) {
  uint8_t  value = program[pc++];
  registers[reg] = value;
}

void LedController::cmd_load_reg_16(uint8_t reg) {
  uint16_t  value = program[pc++] << 8;
  value |= program[pc++];
  registers[reg] = value;
}

void LedController::process_command() {
  uint8_t cmd = program[pc++]; 

  switch(cmd) {

    case CMD_GROUP_SET:
      cmd_group_set();
      break;
      
    case CMD_GROUP_CLEAR:
      cmd_group_clear();
      break;

    case CMD_CLEAR_GROUPS:
      cmd_clear_groups();
      break;

    case CMD_DISABLE_GROUPS:
      cmd_disable_groups();
      break;
          
    case CMD_LOAD_REG_CONST:
      cmd_load_reg_const();
      break;

    case CMD_LDAA8:
      cmd_load_reg_8(0);
      break;
    
    case CMD_LDAB8:
      cmd_load_reg_8(1);
      break;
    
    case CMD_LDAC8:
      cmd_load_reg_8(2);
      break;
    
    case CMD_LDAD8:
      cmd_load_reg_8(3);
      break;
    
    case CMD_LDAE8:
      cmd_load_reg_8(4);
      break;
    
    case CMD_LDAA16:
      cmd_load_reg_16(0);
      break;
    
    case CMD_LDAB16:
      cmd_load_reg_16(1);
      break;
    
    case CMD_LDAC16:
      cmd_load_reg_16(2);
      break;
    
    case CMD_LDAD16:
      cmd_load_reg_16(3);
      break;
        
    case CMD_LDAE16:
      cmd_load_reg_16(4);
      break;
        
    case CMD_LOAD_REG_MAV:
      cmd_load_reg_mav();
      break;
      
    case CMD_PAUSE:
      cmd_pause();
      break;
          
    case CMD_YIELD:
      cmd_yield();
      break;
      
    case CMD_JUMP_ABS:
      cmd_jump_absolute();
      break;

    case CMD_BZ_REL:
      cmd_bz_relative();
      break;
      
    case CMD_BNZ_REL:
      cmd_bnz_relative();
      break;
      
    case CMD_MOVE_REG:
      cmd_move_register();
      break;
               
    case CMD_SETCOLOR:
      cmd_set_color();
      break;    
      
    case CMD_SETFLASH:
      cmd_set_flash();
      break;
      
    case CMD_SETWAVE:
      cmd_set_wave();
      break;
      
    case CMD_SETBOUNCE:
      cmd_set_bounce();
      break;
      
    case CMD_SETRANDOM:
      cmd_set_random();
      break;
      
    case CMD_SETBAR:
      cmd_set_bar();
      break;     
       
    case CMD_SETOFF:
      cmd_set_off();
      break;
      
    case CMD_SETDORMANT:
      cmd_set_dormant();
      break;

    case CMD_0EQ1:
      cmd_0eq1();
      break;

    case CMD_0NE1:
      cmd_0ne1();
      break;

    case CMD_0LT1:
      cmd_0lt1();
      break;
    
    case CMD_0LE1:
      cmd_0le1();
      break;
    
    case CMD_0GT1:
      cmd_0gt1();
      break;
    
    case CMD_0GE1:
      cmd_0ge1();
      break;

    case CMD_PUSH:
      cmd_push();
      break;
      
    case CMD_POP:
      cmd_pop();
      break;
      
    case CMD_AND:
      cmd_and();
      break;

    case CMD_OR:
      cmd_or();
      break;
      
    default:
      if(console != NULL)
      {
        console->console_print("Invalid program instruction %d\r\n", cmd);
      }    
  }
}

void LedController::process_10_millisecond() {
  while(1) {
    if(pausing_time_left > (MS_PER_TIMESLICE/2)) {
      pausing_time_left -= MS_PER_TIMESLICE;
      break;
    }
    if(pc >= program_size) {
      break;
    }
//    if(pc >= sizeof(program)) {
//      break;
//    }
    process_command();
  }
  led_groups->process_10_milliseconds();
  leds->show();
}




