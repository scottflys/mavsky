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
#ifndef MAVCONSOLE_H
#define MAVCONSOLE_H

#include <WProgram.h> 
#include <GCS_MAVLink.h>

#include "MavlinkData.h"

class Logger;
  
class MavConsole {
  char cmd_buffer[128];
  int cmd_index = 0;
  usb_serial_class serial;
  uint8_t led_data_mode = 0;
  
  public:
    MavConsole(usb_serial_class port);
    ~MavConsole();
    void console_print(const char* fmt, ...);
    void do_help();
    const char *on_off(int v);
    void parse_debug_on_off(char* p, int *debug_var_ptr, char *name);
    void do_dump();
    void do_times();
    void do_map_dump();
    void do_map(char* p);
    void do_frsky();
    void do_ldump();
    void do_factory();
    void do_led_map();
    void do_command(char *cmd_buffer);
    uint8_t hex2dec_char(char hex_char);
    uint32_t hex2dec_string(char *hex_string, uint8_t len);
    void process_led_data_line(char *cmd_buffer);
    void check_for_console_command();
    uint8_t atoh(uint8_t c);
};

#endif
