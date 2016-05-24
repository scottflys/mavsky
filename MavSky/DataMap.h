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
#ifndef DATAMAP_H
#define DATAMAP_H

#include <WProgram.h> 
  
#define EEPROM_INIT_VALUE_217           0x68  
#define EEPROM_INIT_VALUE_218           0x69  

#define MAP_SOURCE_RANGEFINDER_DISTANCE 0
#define MAP_SOURCE_BAR_ALTITUDE         1
#define MAP_SOURCE_COUNT                2

#define MAP_TARGET_VARIO_ALTITUDE       0
#define MAP_TARGET_COUNT                1

class DataMap {
  private:
    char* source_name[MAP_SOURCE_COUNT];
    char* target_name[MAP_TARGET_COUNT];
    uint16_t source_for_target[MAP_TARGET_COUNT];
    float scale_for_target[MAP_TARGET_COUNT];    
    int8_t get_source_index(char* name);
    int32_t get_raw_source_value(uint16_t index);
    int8_t get_target_index(char* name);
    int8_t get_scale_power(char* scale_string);
    uint8_t add_map(char *source_name, char* target_name, char* scale_string);
    void console_dump_map();
                
  public:
    DataMap();
    int32_t get_mapped_data(uint8_t target_index);
    const uint16_t VARIO_ALTITUDE = MAP_TARGET_VARIO_ALTITUDE;
    void write_factory_settings();
    void console_map(char*);
};

#endif


