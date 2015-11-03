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
    void do_factory();
    void do_command(char *cmd_buffer);
    void check_for_console_command();

};

#endif
