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


