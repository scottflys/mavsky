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
#ifndef DIAGS_H
#define DIAGS_H

#include <WProgram.h> 

#define MAX_FAULTS 10
  
class Diags {

  const byte LEDPIN = 13;

  uint8_t faults[MAX_FAULTS];
  uint8_t current_flash_codes[MAX_FAULTS];
  uint8_t current_flash_code_count = 0;
  uint8_t current_code_index = 0;
  uint8_t flashes_remaining = 0;
  uint8_t next_flash_state = 0;           
  
  public:
    Diags();
    void set_fault_to(uint8_t fault_id, uint8_t fault_value);
    void clear_fault(uint8_t fault_id);
    void clear_all_faults();
    void load_new_faults();
    void update_led();
};

#endif


