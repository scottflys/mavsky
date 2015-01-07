//  Copyright (c) Scott Simpson 2015
//  
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
//
//  For Teensy 3.1 support
//    Connect:
//      SPort S     -> TX1
//      SPort +     -> Vin
//      SPort -     -> GND
//  
//      Mavlink TX  -> RX2
//      Mavlink GND -> GND
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <GCS_MAVLink.h>
#include "MavSky.h"
#include "FrSkySPort.h"
#include "Logger.h"
#include "MavLink.h"

#define PRODUCT_STRING  "MAVSky Version 1.1.1"

#define DEBUG_SERIAL    Serial
#define MAVLINK_SERIAL  Serial2
#define LEDPIN          13
                          
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void console_print(char* fmt, ...) {
    char formatted_string[256];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(formatted_string, fmt, argptr);
    va_end(argptr);
    DEBUG_SERIAL.print(formatted_string);
}

void setup()  {
  debug_init();
  delay(5000);

  pinMode(LEDPIN, OUTPUT);
  console_print("%s\r\nStarting\r\n]", PRODUCT_STRING);

  telem_data_init();  
  frsky_init();
  mavlink_init();
}

void check_for_faults() {
  int mav_online;
  mav_online = mavlink_online();
  diags_set_fault_to(FAULT_MAV_OFFLINE, !mav_online);
  if(mav_online) {                                                                  // don't set other mav faults if mav is offline
    diags_set_fault_to(FAULT_MAV_SYS_STATUS, !mavlink_sys_status_data_valid());
    diags_set_fault_to(FAULT_MAV_GPS, !mavlink_gps_data_valid());
    diags_set_fault_to(FAULT_MAV_VFR_HUD, !mavlink_vfr_data_valid());
    diags_set_fault_to(FAULT_MAV_RAW_IMU, !mavlink_imu_data_valid());
    diags_set_fault_to(FAULT_MAV_ATTITUDE, !mavlink_attitude_data_valid());
  } else {
    diags_clear_fault(FAULT_MAV_SYS_STATUS);
    diags_clear_fault(FAULT_MAV_GPS);
    diags_clear_fault(FAULT_MAV_VFR_HUD);
    diags_clear_fault(FAULT_MAV_RAW_IMU);
    diags_clear_fault(FAULT_MAV_ATTITUDE);
  }
  diags_set_fault_to(FAULT_SPORT_OFFLINE, !frsky_online());
}

uint32_t next_200_loop = 0L;
uint32_t next_100_loop = 0L;

void loop()  {
  uint16_t len;
  uint32_t current_milli;
  
  process_mavlink_packets();

  frsky_process();               // Check FrSky S.Port communication

  check_for_console_command();
  
  current_milli = millis();

  if(current_milli >= next_200_loop) {
    next_200_loop = current_milli + 200;
    diags_update_led();
  }
  
  if(current_milli >= next_100_loop) {
    next_100_loop = current_milli + 100;
    check_for_faults();
  }
}








