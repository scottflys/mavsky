//#include <OctoWS2811.h>
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
//  Creator:  Scott Simpson
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
//
//  Required Connections
//  --------------------
//    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
//    pin 14: LED strip #2    All 8 are the same length.
//    pin 7:  LED strip #3
//    pin 8:  LED strip #4    A 100 ohm resistor should used
//    pin 6:  LED strip #5    between each Teensy pin and the
//    pin 20: LED strip #6    wire to the LED strip, to minimize
//    pin 21: LED strip #7    high frequency ringining & noise.
//    pin 5:  LED strip #8
//    pin 15 & 16 - Connect together, but do not use
//    pin 4 - Do not use
//    pin 3 - Do not use as PWM.  Normal use is ok.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <GCS_MAVLink.h>
#include <EEPROM.h>
#include "MavSky.h"
#include "FrSkySPort.h"
#include "MavConsole.h"
#include "Diags.h"
#include "Logger.h"
#include "DataBroker.h"

#include "Led.h"

#define LEDPIN          13
                          
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Diags diags;
Logger *logger;
MavConsole *console;
MavLinkData *mav;
FrSkySPort *frsky;
DataBroker *data_broker;

DMAMEM int displayMemory[MAX_LEDS_PER_STRIP*6];
int drawingMemory[MAX_LEDS_PER_STRIP*6];
  
LedController* led_strip_ptr;

void setup()  {
  console = new MavConsole(Serial);
  logger = new Logger();
  mav = new MavLinkData();
  frsky = new FrSkySPort();
  data_broker = new DataBroker();

  delay(5000);

  pinMode(LEDPIN, OUTPUT);
  console->console_print("%s\r\nStarting\r\n]", PRODUCT_STRING);

  led_strip_ptr = new LedController();  
}

void check_for_faults() {
  int mav_online;
  mav_online = mav->mavlink_heartbeat_data_valid();
  diags.set_fault_to(FAULT_MAV_OFFLINE, !mav_online);
  if(mav_online) {                                                                  // don't set other mav faults if mav is offline
    diags.set_fault_to(FAULT_MAV_SYS_STATUS, !mav->mavlink_sys_status_data_valid());
    diags.set_fault_to(FAULT_MAV_GPS, !mav->mavlink_gps_data_valid());
    diags.set_fault_to(FAULT_MAV_VFR_HUD, !mav->mavlink_vfr_data_valid());
    diags.set_fault_to(FAULT_MAV_RAW_IMU, !mav->mavlink_imu_data_valid());
    diags.set_fault_to(FAULT_MAV_ATTITUDE, !mav->mavlink_attitude_data_valid());
  } else {
    diags.clear_fault(FAULT_MAV_SYS_STATUS);
    diags.clear_fault(FAULT_MAV_GPS);
    diags.clear_fault(FAULT_MAV_VFR_HUD);
    diags.clear_fault(FAULT_MAV_RAW_IMU);
    diags.clear_fault(FAULT_MAV_ATTITUDE);
  }
  diags.set_fault_to(FAULT_SPORT_OFFLINE, !frsky->frsky_online());
}

uint32_t next_1000_loop = 0L;
uint32_t next_200_loop = 0L;
uint32_t next_100_loop = 0L;
uint32_t next_10_loop = 0L;

void loop()  {
  uint32_t current_milli = millis();
  uint8_t state = (current_milli % 10);

  switch(state) {
    case 0:
      mav->process_mavlink_packets();
    
      frsky->frsky_process();         
    
      console->check_for_console_command();  
    
      if(current_milli >= next_1000_loop) {
        next_1000_loop = current_milli + 1000;
        mav->process_1000_millisecond();
      }
      
      if(current_milli >= next_200_loop) {
        next_200_loop = current_milli + 200;
        diags.update_led();
      }
      
      if(current_milli >= next_100_loop) {
        next_100_loop = current_milli + 100;
        if(current_milli > 10000) {
          check_for_faults();
        }
        mav->process_100_millisecond();   
      }
      break;
      
   case 1:
   case 2:
   case 3:
   case 4:
     if(current_milli >= next_10_loop) {
       next_10_loop = current_milli + 10;
       led_strip_ptr->process_10_millisecond();
     }
     break;
     
   default:                                           // leave states 5-9 dormant to reduce LED flickering
     break;
  }
}








