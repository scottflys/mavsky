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
#ifndef MAVSKY_H
#define MAVSKY_H

#define PRODUCT_STRING  "MAVSky Version 2.1.17.5"

#define FAULT_MAV_OFFLINE               1
#define FAULT_SPORT_OFFLINE             2
#define FAULT_MAV_SYS_STATUS            3
#define FAULT_MAV_GPS                   4 
#define FAULT_MAV_VFR_HUD               5
#define FAULT_MAV_RAW_IMU               6
#define FAULT_MAV_ATTITUDE              7

// EEPROM
#define EEPROM_ADDR_VERSION             0
#define EEPROM_ADDR_FRSKY_VFAS_ENABLE   1
#define EEPROM_ADDR_MAP_BEGIN          32         // EEPROM address for start of map

#define EEPROM_LED_MAP_BASE            64         // LED code start

#define EEPROM_LED_CODE_SIZE           72         // LED code size
#define EEPROM_LED_CODE_BASE           74         // LED code start
#define EEPROM_LED_CODE_MAX_SIZE       (2048-EEPROM_LED_CODE_BASE)      

#define MAX_STRIPS                      8
#define MAX_REGISTERS                   8
#define MAX_STACK_SIZE                 32
#define MAX_LEDS_PER_STRIP             16

#define MAX_LED_GROUPS                 20      
#define MAX_LEDS_PER_GROUP            160
#define MAX_LED_ACTIONS_PER_GROUP      20
#define MAX_LED_LAYERS                 50       



#endif
