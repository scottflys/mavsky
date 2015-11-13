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
#ifndef DATABROKER_H
#define DATABROKER_H

#include <WProgram.h> 
#include "ExtendedMessage.h"
#include "DataMap.h" 
 
class DataBroker {
  private:
    
  public:
    DataBroker();
    static void get_vario_data(int32_t*, int32_t*);
    static void get_fas_data(uint32_t *p1, uint32_t *p2);
    static void get_gps_data(uint32_t *lon, uint32_t *lat, int32_t *alt, uint32_t *speed, uint32_t *heading);
    static void get_rpm_data(uint32_t *rpm);
    static void get_sp2uh_data(uint32_t *fuel);
    static ExtendedMessage extended_message_processor;
    static DataMap data_map;
    void write_factory_settings();
    void console_map(char*);
};

#endif


