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
#include <EEPROM.h>
#include "MavSky.h"
#include "DataMap.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;

uint8_t   default_program[] = {
32,  0,  0,  7,  0,  0,  7,  
32,  0,  8,  15,  1,  0,  7,  
32,  0,  16,  23,  2,  0,  7,  
32,  0,  24,  31,  3,  0,  7,  
32,  0,  32,  39,  4,  0,  7,  
32,  0,  40,  47,  5,  0,  7,  
32,  0,  48,  55,  6,  0,  7,  
32,  0,  56,  63,  7,  0,  7,  
32,  1,  0,  0,  0,  0,  0,  
32,  1,  1,  2,  1,  0,  1,  
32,  1,  3,  5,  2,  0,  2,  
32,  1,  6,  9,  3,  0,  3,  
32,  1,  10,  14,  4,  0,  4,  
32,  1,  15,  20,  5,  0,  5,  
32,  1,  21,  27,  6,  0,  6,  
32,  1,  28,  35,  7,  0,  7,  
32,  2,  0,  7,  0,  0,  7,  
32,  2,  8,  15,  1,  0,  7,  
32,  3,  0,  7,  6,  0,  7,  
32,  3,  8,  15,  7,  0,  7,  
32,  4,  0,  7,  2,  0,  7,  
32,  4,  8,  15,  3,  0,  7,  
32,  4,  16,  23,  4,  0,  7,  
32,  4,  24,  31,  5,  0,  7,  
32,  5,  0,  7,  0,  0,  7,  
32,  6,  0,  7,  1,  0,  7,  
32,  7,  0,  7,  6,  0,  7,  
32,  8,  0,  7,  7,  0,  7,  
32,  9,  0,  7,  0,  0,  7,  
32,  9,  8,  15,  2,  0,  7,  
32,  9,  16,  23,  4,  0,  7,  
32,  9,  24,  31,  6,  0,  7,  
32,  10,  0,  7,  1,  0,  7,  
32,  10,  8,  15,  3,  0,  7,  
32,  10,  16,  23,  5,  0,  7,  
32,  10,  24,  31,  7,  0,  7,  
35,  
2,  0,  2,  73,  4,  76,  19,  9,  0,  10,  0,  13,  0,  16,  
1,  0,  0,  16,  16,  16,  48,  0,  
1,  0,  0,  16,  0,  0,  48,  1,  
72,  4,  76,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  4,  176,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  39,  
72,  0,  0,  48,  0,  
1,  0,  0,  255,  255,  0,  73,  0,  30,  74,  3,  202,  75,  0,  0,  49,  0,  
1,  0,  0,  255,  255,  0,  73,  0,  20,  74,  3,  212,  75,  0,  200,  49,  0,  
72,  4,  176,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  5,  20,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  82,  
72,  0,  200,  73,  0,  10,  51,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  5,  
72,  255,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  6,  
1,  0,  0,  255,  0,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  7,  
72,  255,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  8,  
72,  5,  20,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  5,  120,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  101,  
72,  0,  0,  48,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  150,  74,  3,  232,  75,  0,  1,  55,  5,  
72,  255,  0,  73,  0,  150,  74,  3,  232,  75,  0,  1,  55,  6,  
1,  0,  0,  255,  0,  0,  73,  0,  150,  74,  3,  232,  75,  0,  0,  55,  7,  
72,  255,  0,  73,  0,  150,  74,  3,  232,  75,  0,  0,  55,  8,  
1,  0,  0,  100,  100,  100,  73,  0,  30,  74,  3,  202,  75,  0,  0,  49,  0,  
1,  0,  0,  100,  100,  100,  73,  0,  20,  74,  3,  212,  75,  0,  200,  49,  0,  
72,  5,  120,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  5,  220,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  70,  
72,  0,  200,  73,  0,  10,  51,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  50,  74,  0,  2,  75,  0,  1,  50,  5,  
72,  255,  0,  73,  0,  50,  74,  0,  2,  75,  0,  1,  50,  6,  
1,  0,  0,  255,  0,  0,  73,  0,  50,  74,  0,  2,  75,  0,  0,  50,  7,  
72,  255,  0,  73,  0,  50,  74,  0,  2,  75,  0,  0,  50,  8,  
72,  5,  220,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  6,  64,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  58,  
1,  0,  0,  5,  5,  5,  48,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  100,  74,  0,  1,  53,  5,  
72,  255,  0,  73,  0,  100,  74,  0,  1,  53,  6,  
1,  0,  0,  255,  0,  0,  73,  0,  100,  74,  0,  1,  53,  7,  
72,  255,  0,  73,  0,  100,  74,  0,  1,  53,  8,  
72,  6,  64,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  6,  164,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  67,  
72,  0,  0,  48,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  20,  74,  3,  182,  75,  0,  0,  49,  9,  
1,  0,  0,  255,  0,  0,  73,  0,  20,  74,  3,  182,  75,  0,  100,  49,  9,  
72,  255,  0,  73,  0,  20,  74,  3,  182,  75,  0,  200,  49,  10,  
72,  255,  0,  73,  0,  20,  74,  3,  182,  75,  1,  44,  49,  10,  
72,  6,  164,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  7,  8,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  38,  
1,  0,  0,  85,  85,  85,  48,  2,  
1,  0,  0,  85,  0,  0,  48,  3,  
2,  0,  80,  73,  0,  20,  20,  9,  0,  10,  0,  13,  0,  8,  
1,  0,  0,  255,  0,  0,  48,  3,  
72,  7,  8,  2,  1,  2,  18,  9,  0,  2,  0,  2,  73,  7,  108,  19,  9,  0,  10,  0,  10,  1,  12,  9,  0,  10,  0,  13,  0,  5,  
72,  0,  0,  48,  0,  
72,  7,  108,  2,  1,  2,  18,  9,  0,  10,  0,  13,  0,  82,  
1,  0,  0,  200,  200,  200,  48,  0,  
1,  0,  0,  255,  0,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  5,  
72,  255,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  6,  
1,  0,  0,  255,  0,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  7,  
72,  255,  0,  73,  0,  25,  74,  0,  0,  75,  0,  100,  76,  0,  1,  52,  8,  
4,  5,  0,  252,  
};

DataMap::DataMap() {
  source_name[MAP_SOURCE_RANGEFINDER_DISTANCE] = (char*)"rangefinder_distance";
  source_name[MAP_SOURCE_BAR_ALTITUDE] = (char*)"bar_altitude";

  target_name[MAP_TARGET_VARIO_ALTITUDE] = (char*)"vario_altitude";
  
  for(uint8_t i=0; i<MAP_TARGET_COUNT; i++) {
    source_for_target[i] = EEPROM.read(EEPROM_ADDR_MAP_BEGIN + i*2);
    int8_t scale_power = EEPROM.read(EEPROM_ADDR_MAP_BEGIN + i*2 + 1);
    scale_for_target[i] = powf(10.0, scale_power);    
  }
  if(EEPROM.read(EEPROM_ADDR_VERSION) != EEPROM_INIT_VALUE_217) {
    write_factory_settings();
    EEPROM.write(EEPROM_ADDR_VERSION, EEPROM_INIT_VALUE_217);
  }
}

void DataMap::write_factory_settings() {
  EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);
  add_map((char*)"bar_altitude", (char*)"vario_altitude", (char*)"100.0");  
  add_map((char*)"climb_rate", (char*)"vario_vertical_speed", (char*)"100.0");  

  for(uint16_t i=0; i<8; i++) {
    EEPROM.write(EEPROM_LED_MAP_BASE + i, i);
  } 
    
  EEPROM.write(EEPROM_LED_CODE_SIZE, (sizeof(default_program) >> 8) & 0xff);
  EEPROM.write(EEPROM_LED_CODE_SIZE + 1, sizeof(default_program) & 0xff);
  for(uint16_t i=0; i<sizeof(default_program); i++) {
    EEPROM.write(EEPROM_LED_CODE_BASE + i, default_program[i]);
  }  
}

void DataMap::console_map(char* p) {        // todo - don't need this param
  char* p1 = strtok(NULL, " ");
  if(p1 == NULL) {  
    console_dump_map();
  } else { 
    char* p2 = strtok(NULL, " ");
    if(p2 == NULL) { 
      console->console_print("Command requires at least two parameters.  Please retry\r\n");
    } else {
      char* p3 = strtok(NULL, " ");
      if(p3 == NULL) {
        if(!add_map(p1, p2, (char*)"1.0")) { 
          console->console_print("Command was not understood.  Please check syntax and retry\r\n"); 
        } else {
          console->console_print("%s is now mapped to %s with a scale of 1.0\r\n", p1, p2); 
        }
      } else { 
        if(!add_map(p1, p2, p3)) {
          console->console_print("Command was not understood.  Please check syntax and retry\r\n");           
        } else {
          console->console_print("%s is now mapped to %s with a scale of %s\r\n", p1, p2, p3);    
        }
      }
    }
  }
}

void DataMap::console_dump_map() {
   for(uint8_t target_index=0; target_index<MAP_TARGET_COUNT; target_index++) {
     uint8_t source_index = source_for_target[target_index];
     console->console_print("%s -> %s (scale:%f)\r\n", source_name[source_index], target_name[target_index], scale_for_target[target_index]);
  } 
}

uint8_t DataMap::add_map(char *source_name, char* target_name, char* scale_string) {
  int8_t source_index = get_source_index(source_name);
  int8_t target_index = get_target_index(target_name);
  int8_t scale_power = get_scale_power(scale_string);
  if(source_index < 0 || target_index < 0) {
    return 0;
  } else {
    source_for_target[target_index] = source_index;
    EEPROM.write(EEPROM_ADDR_MAP_BEGIN + target_index*2, source_index);  
    scale_for_target[target_index] = powf(10.0, scale_power); 
    EEPROM.write(EEPROM_ADDR_MAP_BEGIN + target_index*2 + 1, scale_power);
    return 1;
  }
}

int32_t DataMap::get_mapped_data(uint8_t target_index) {
  uint8_t source_index = source_for_target[target_index];
  int32_t raw_value = get_raw_source_value(source_index);
  float scale_value = scale_for_target[target_index];
  int32_t scaled_value = (int32_t)lroundf((float)raw_value * scale_value);
  return scaled_value; 
}

int8_t DataMap::get_source_index(char* name) {
  for(uint8_t i=0; i<MAP_SOURCE_COUNT; i++) {
    if(strcmp(source_name[i], name) == 0) {
      return i;
    }
  }
  return -1;
}

int8_t DataMap::get_target_index(char* name) {
  for(uint8_t i=0; i<MAP_TARGET_COUNT; i++) {
    if(strcmp(target_name[i], name) == 0) {
      return i;
    }
  }
  return -1;
}

int8_t DataMap::get_scale_power(char* scale_string) {
  float scale = atof(scale_string);
  float power_float = log10f(scale);
  int8_t power_int = (int8_t)lroundf(power_float);
  return power_int;
}

int32_t DataMap::get_raw_source_value(uint16_t index) {
  switch(index) {
    case MAP_SOURCE_RANGEFINDER_DISTANCE:
      return mav->rangefinder_distance;
    case MAP_SOURCE_BAR_ALTITUDE:
      return (int32_t)mav->bar_altitude;     
    default:
      break;
  }
  return 0;
}


