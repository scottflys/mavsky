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
#include "OctoWS2811.h" 

#define GROUP_MODE_DISABLED    0
#define GROUP_MODE_SOLID       1
#define GROUP_MODE_FLASH       2
#define GROUP_MODE_WAVE        3
#define GROUP_MODE_RANDOM      4
#define GROUP_MODE_BAR         5
#define GROUP_MODE_BOUNCE      6
#define GROUP_MODE_FILL        7

class LedController {
  private:
    uint16_t  pc = 0;
    uint16_t  current_instruction_pc = 0;
    OctoWS2811* leds;
    int32_t pausing_time_left = 0; 
    uint32_t get_variable(uint16_t input);
    uint32_t registers[MAX_REGISTERS];
    uint32_t stack[MAX_STACK_SIZE];
    uint8_t stack_size = 0;
    void cmd_group_set();
    void cmd_group_clear();
    void cmd_clear_groups();
    void cmd_disable_actions();
    void cmd_load_reg_const();
    void cmd_load_reg_const8();   
    void cmd_load_reg_mav();
    void cmd_pause();
    void cmd_yield();
    void cmd_jump_absolute();
    void cmd_jump_relative();
    void cmd_bz_relative();
    void cmd_bnz_relative();
    
    void cmd_move_register();
    void cmd_set_color();
    void cmd_set_flash();
    void cmd_set_wave();
    void cmd_set_bounce();
    void cmd_set_random();
    void cmd_set_bar();
    void cmd_set_off();
    void cmd_set_fill();

    void cmd_0eq1();
    void cmd_0ne1();
    void cmd_0lt1();
    void cmd_0le1();
    void cmd_0gt1();
    void cmd_0ge1();
    
    void cmd_and();
    void cmd_or();
    void cmd_push();
    void cmd_pop();
    void cmd_load_reg_8(uint8_t);
    void cmd_load_reg_16(uint8_t);
    
  public:
    LedController();
    void reload();
    void process_10_millisecond();
    void process_command();
    void dump_diags();
    void update_leds();
};

#endif


