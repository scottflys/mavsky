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
#define VAR_RC8                 8
#define VAR_BATTERY_REMAINING   9

#define VALUE_VARIABLE          1
#define SCALAR_VARIABLE         2

#define COND_EQUAL              1
#define COND_NOT_EQUAL          2
#define COND_LESS_THAN          3
#define COND_GREATER_THAN       4
#define COND_LESS_THAN_EQUAL    5
#define COND_GREATER_THAN_EQUAL 6

#define CMD_LOAD_REG_CONST      1                 // rr cccccccc                     rr = register number, cccccccc = constant value
#define CMD_LOAD_REG_MAV        2                 // rr mm                           rr = register number, mm = mav value
#define CMD_PAUSE               3                 // rr                              rr = register with milliseconds               
#define CMD_YIELD               4                 //                                         
#define CMD_JUMP_ABS            5                 // aaaa                            aaaa = absolute jump
#define CMD_COND_JUMP_ABS       6                 // aaaa                            aaaa = absolute jump
#define CMD_COND_JUMP_REL       7                 // jjjj                            jjjj = relative jump
#define CMD_MOVE_REG            8                 // sr tr                           sr = source register, tr = target register                                                                    // todo test
#define CMD_PUSH                9                 // sr                              sr = source register
#define CMD_POP                 10                // tr                              tr = target register
#define CMD_OR                  11                // 
#define CMD_AND                 12                // 

#define CMD_0EQ1                16                // 0 = 0 == 1
#define CMD_0NE1                17                // 0 = 0 != 1
#define CMD_0LT1                18                // 0 = 0 < 1
#define CMD_0LE1                19                // 0 = 0 <= 1
#define CMD_0GT1                20                // 0 = 0 > 1
#define CMD_0GE1                21                // 0 = 0 >= 1

#define CMD_GROUP_SET           32                // gn gs ge ln ls le               gn = group number, gs = group start, ge = group end, ln = led strip number, ls = led start, le = led end  
#define CMD_GROUP_CLEAR         33                // gn                              gn = group number
#define CMD_CLEAR_GROUPS        34                //                              

#define CMD_SETCOLOR            48                // gg cr                           gg = group number, cr = color register
#define CMD_SETFLASH            49                // gg cr nr fr                     gg = group number, oc = on color register, fc = off color register, nr = on time register, fr = off time register
#define CMD_SETWAVE             50                // gg rr or fc tr wr               gg = group number, dr = reverse register, oc = on color register, fc = off color register, tr = state time register, wr = on width register
#define CMD_SETRANDOM           51                // gg ct in                        gg = group number, ct = change time register, intensity
#define CMD_SETBAR              52                // gg dr oc fc pr                  gg = group number, dr = reverse register, oc = on color register, fc = off color register, pr = percent register  
#define CMD_SETBOUNCE           53                // gg                              gg = group number (implied: R0 = reverse, R1 = on color, R2 = off color, R3 = state time, R4 = on width)

#define CMD_LDAA8               64                // cc                              cc = short constant
#define CMD_LDAB8               65                // cc                              cc = short constant
#define CMD_LDAC8               66                // cc                              cc = short constant
#define CMD_LDAD8               67                // cc                              cc = short constant
#define CMD_LDAE8               68                // cc                              cc = short constant
#define CMD_LDAF8               69                // cc                              cc = short constant
#define CMD_LDAG8               70                // cc                              cc = short constant
#define CMD_LDAH8               71                // cc                              cc = short constant

#define RIGHT_FRONT 0
#define LEFT_REAR   1
#define LEFT_FRONT  2
#define RIGHT_REAR  3

uint8_t program[EEPROM_LED_CODE_MAX_SIZE];
uint16_t program_size = 0;
LedGroups* led_groups;

uint8_t   program_default[] = {
  CMD_YIELD,
    
// if mav->rc8 < 1050

  CMD_LOAD_REG_MAV, 0, VAR_RC8, 
  CMD_LOAD_REG_CONST, 1, 0, 0, 4, 0x1a,             // 1050
  CMD_0GE1,
  CMD_COND_JUMP_REL, 0, 90, 
  
    // the following block is 90 
    CMD_CLEAR_GROUPS,
    CMD_GROUP_SET, 0, 0, 0, 0, 0, 0,   
    CMD_GROUP_SET, 1, 0, 1, 1, 0, 1,   
    CMD_GROUP_SET, 2, 0, 2, 2, 0, 2,   
    CMD_GROUP_SET, 3, 0, 3, 3, 0, 3,   
    CMD_GROUP_SET, 4, 0, 4, 4, 0, 4,   
    CMD_GROUP_SET, 5, 0, 5, 5, 0, 5,   
    CMD_GROUP_SET, 6, 0, 6, 6, 0, 6,   
    CMD_GROUP_SET, 7, 0, 7, 7, 0, 7,   
  
    CMD_LOAD_REG_CONST, 0, 0, 0x20, 0, 0,   
    CMD_SETCOLOR, 0, 0, 
    CMD_SETCOLOR, 1, 0, 
    CMD_SETCOLOR, 2, 0, 
    CMD_SETCOLOR, 3, 0, 
    CMD_SETCOLOR, 4, 0, 
    CMD_SETCOLOR, 5, 0, 
    CMD_SETCOLOR, 6, 0, 
    CMD_SETCOLOR, 7, 0, 
  
    CMD_JUMP_ABS, 0, 0,  

// if mav->rc8 < 1150

  CMD_LOAD_REG_MAV, 0, VAR_RC8, 
  CMD_LOAD_REG_CONST, 1, 0, 0, 4, 0x7e,             // 1150
  CMD_0GE1,
  CMD_COND_JUMP_REL, 0, 156,

  // the following block is 156
    CMD_CLEAR_GROUPS,
    CMD_GROUP_SET, 0, 0, 6, 2, 0, 6,  
    CMD_GROUP_SET, 1, 0, 6, 1, 0, 6,  
    CMD_GROUP_SET, 2, 0, 6, 5, 0, 6,  
    CMD_GROUP_SET, 3, 0, 6, 4, 0, 6,  
    CMD_GROUP_SET, 4, 0, 6, 0, 0, 6,  
    CMD_GROUP_SET, 5, 0, 6, 3, 0, 6,  
  
    CMD_GROUP_SET, 6, 0, 0, 2, 7, 7,  
    CMD_GROUP_SET, 7, 0, 0, 1, 7, 7,  
    CMD_GROUP_SET, 8, 0, 0, 5, 7, 7,  
    CMD_GROUP_SET, 9, 0, 0, 4, 7, 7,  
    CMD_GROUP_SET, 10, 0, 0, 0, 7, 7,  
    CMD_GROUP_SET, 11, 0, 0, 3, 7, 7,  
  
    CMD_GROUP_SET, 12, 0, 7, 6, 0, 7,  
    CMD_GROUP_SET, 13, 0, 7, 7, 0, 7,  


    CMD_LOAD_REG_CONST, 0, 0, 255, 255, 255,   
    CMD_SETCOLOR, 0, 0, 
    CMD_SETCOLOR, 1, 0, 
    CMD_SETCOLOR, 2, 0, 
    CMD_SETCOLOR, 3, 0, 
    CMD_SETCOLOR, 4, 0, 
    CMD_SETCOLOR, 5, 0, 

    CMD_LOAD_REG_CONST, 0, 0, 255, 0, 0,   
    CMD_SETCOLOR, 6, 0, 
    CMD_SETCOLOR, 7, 0, 
    CMD_SETCOLOR, 8, 0, 

    CMD_LOAD_REG_CONST, 0, 0, 0, 255, 0,   
    CMD_SETCOLOR, 9, 0, 
    CMD_SETCOLOR, 10, 0, 
    CMD_SETCOLOR, 11, 0, 

// todo -- what about legs

    CMD_JUMP_ABS, 0, 0,            

// if mav->rc8 < 1250  

  CMD_LOAD_REG_MAV, 0, VAR_RC8, 
  CMD_LOAD_REG_CONST, 1, 0, 0, 4, 0xe2,             // 1250
  CMD_0GE1,
  CMD_COND_JUMP_REL, 0, 145, 

  // the following block is 145
    CMD_GROUP_SET, 0, 0, 7, 0, 0, 7,   
    CMD_GROUP_SET, 1, 0, 7, 1, 0, 7,   
    CMD_GROUP_SET, 2, 0, 7, 2, 0, 7,   
    CMD_GROUP_SET, 3, 0, 7, 3, 0, 7,   
    CMD_GROUP_SET, 4, 0, 7, 4, 0, 7,   
    CMD_GROUP_SET, 5, 0, 7, 5, 0, 7,   
    CMD_GROUP_SET, 6, 0, 7, 6, 0, 7,   
    CMD_GROUP_SET, 7, 0, 7, 7, 0, 7,   

    CMD_LOAD_REG_CONST, 0, 0, 0, 0, 0,             // reverse
    CMD_LOAD_REG_CONST, 1, 0, 20, 0, 0,            // on color
    CMD_LOAD_REG_CONST, 2, 0, 0, 0, 0,             // off color
    CMD_LOAD_REG_CONST, 3, 0, 0, 0, 100,           // state time
    CMD_LOAD_REG_CONST, 4, 0, 0, 0, 3,             // on width
    CMD_SETWAVE, 0, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 1, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 2, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 3, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 4, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 5, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 6, 0, 1, 2, 3, 4,   
    CMD_SETWAVE, 7, 0, 1, 2, 3, 4,   

    CMD_JUMP_ABS, 0, 0,   

};
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
        
LedController::LedController() {
  if(program_size == 0) {
    uint16_t n = (EEPROM.read(EEPROM_LED_CODE_SIZE) << 8) + EEPROM.read(EEPROM_LED_CODE_SIZE+1);
    if(n < EEPROM_LED_CODE_MAX_SIZE) {
      program_size = n;
      for(uint16_t i=0; i<program_size; i++) {
        program[i] = EEPROM.read(EEPROM_LED_CODE_BASE + i);
      }
    }  
  }
  leds = new OctoWS2811(MAX_LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
  leds->begin();

  for (int i=0; i < MAX_LEDS_PER_STRIP*MAX_STRIPS; i++) {
    leds->setPixel(i, 0x000000);
  }
  leds->show();

  led_groups = new LedGroups(leds);
}

uint32_t LedController::get_variable(uint16_t input) {
  switch(input) {
    case VAR_RC8:
      return mav->rc8;
      break;
    case VAR_BATTERY_REMAINING:
      return mav->battery_remaining;
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
  uint8_t  strip_number = program[pc++];
  uint8_t  strip_led_start = program[pc++];
  uint8_t  strip_led_end = program[pc++];

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
          group_ptr->strip_number[group_i] = strip_number;
          group_ptr->led_position[group_i] = strip_i;      
          if(group_i >= group_ptr->led_count) {
            group_ptr->led_count = group_i + 1;
          }
          strip_i += strip_increment;
        }        
      } else {
        for(int group_i = group_led_start; group_i >= group_led_end; group_i--) {
          group_ptr->strip_number[group_i] = strip_number;
          group_ptr->led_position[group_i] = strip_i;
          if(group_i >= group_ptr->led_count) {
            group_ptr->led_count = group_i + 1;
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

void LedController::cmd_jump_abbsolute() {
  uint16_t next_pc = program[pc++] << 8;                             
  pc = next_pc;
}

void LedController::cmd_cond_jump_absolute() {
  uint16_t next_pc = program[pc++] << 8;                             
  next_pc |= program[pc++];                          
  if(registers[0] != 0) {
    pc = next_pc;
  }
}

void LedController::cmd_cond_jump_relative() {
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
  uint8_t group_number = program[pc++];
  uint8_t on_color_register_number = program[pc++];
  if(on_color_register_number < MAX_REGISTERS && group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_solid(registers[on_color_register_number]);
  }  
}

void LedController::cmd_set_flash() {
  uint8_t group_number = program[pc++];
  uint8_t on_color_register_number = program[pc++];
  uint8_t  off_color_register_number = program[pc++];
  uint8_t on_time_register_number = program[pc++];
  uint8_t off_time_register_number = program[pc++];
  if(on_color_register_number < MAX_REGISTERS && off_color_register_number < MAX_REGISTERS && on_time_register_number < MAX_REGISTERS && off_time_register_number < MAX_REGISTERS && group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_flash(registers[on_color_register_number], registers[off_color_register_number], registers[on_time_register_number], registers[off_time_register_number]);  
  }
}

void LedController::cmd_set_wave() {
  uint8_t group_number = program[pc++];
  uint8_t reverse_register_number = program[pc++];
  uint8_t on_color_register_number = program[pc++];
  uint8_t  off_color_register_number = program[pc++];
  uint8_t state_time_register_number = program[pc++];
  uint8_t on_width_register_number = program[pc++];
  if(group_number < led_groups->led_group_count && reverse_register_number < MAX_REGISTERS && on_color_register_number < MAX_REGISTERS && off_color_register_number < MAX_REGISTERS && state_time_register_number < MAX_REGISTERS && on_width_register_number < MAX_REGISTERS) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_wave((uint8_t)(registers[reverse_register_number]), registers[on_color_register_number], registers[off_color_register_number], registers[state_time_register_number], registers[on_width_register_number]);  
  }
}
  
void LedController::cmd_set_bounce() {
  uint8_t group_number = program[pc++];
  if(group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_bounce((uint8_t)(registers[0]), registers[1], registers[2], registers[3], registers[4]);  
  }
}
    
void LedController::cmd_set_random() {
  uint8_t group_number = program[pc++];
  uint8_t state_time_register_number = program[pc++];
  uint8_t intensity = program[pc++];
  if(state_time_register_number < MAX_REGISTERS && group_number < led_groups->led_group_count) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_random(registers[state_time_register_number], intensity);  
  }
}
        // gg dr oc fc pr 
void LedController::cmd_set_bar() {
  uint8_t group_number = program[pc++];
  uint8_t reverse_register_number = program[pc++];                    
  uint8_t on_color_register_number = program[pc++];
  uint8_t off_color_register_number = program[pc++];
  uint8_t percent_register_number = program[pc++];
  if(group_number < led_groups->led_group_count && reverse_register_number < MAX_REGISTERS && on_color_register_number < MAX_REGISTERS && off_color_register_number < MAX_REGISTERS && percent_register_number < MAX_REGISTERS) {
    LedGroup* group_ptr = led_groups->get_led_group(group_number);
    group_ptr->set_bar((uint8_t)(registers[reverse_register_number]), registers[on_color_register_number], registers[off_color_register_number], registers[percent_register_number]);  
  }
}

void LedController::cmd_0lt1() {
  if(registers[0] <  registers[1]) {
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
    
    case CMD_LDAF8:
      cmd_load_reg_8(5);
      break;
    
    case CMD_LDAG8:
      cmd_load_reg_8(6);
      break;
    
    case CMD_LDAH8:
      cmd_load_reg_8(7);
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
      cmd_jump_abbsolute();
      break;

    case CMD_COND_JUMP_ABS:
      cmd_cond_jump_absolute();
      break;
      
    case CMD_COND_JUMP_REL:
      cmd_cond_jump_relative();
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

    case CMD_0LT1:
      cmd_0lt1();
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
    process_command();
  }
  for(int i=0; i<led_groups->led_group_count; i++) {
    LedGroup* group_pnt = led_groups->get_led_group(i);
    group_pnt->process_10_millisecond();
  }
  leds->show();
}




