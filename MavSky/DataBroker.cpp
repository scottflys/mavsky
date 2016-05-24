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
#include "DataBroker.h"
#include "FrSkySPort.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern FrSkySPort *frsky;
extern MavLinkData *mav;
extern MavConsole *console;
extern uint16_t telem_next_extension_word();

ExtendedMessage DataBroker::extended_message_processor;
DataMap DataBroker::data_map;

DataBroker::DataBroker() {
  //mav->mavlink_sys_status_data_valid()

  if(EEPROM.read(EEPROM_ADDR_FRSKY_VFAS_ENABLE)) {
    frsky->set_fas_request_callback(get_fas_data);
  }  
  if(EEPROM.read(EEPROM_ADDR_FRSKY_VARIO_ENABLE)) {
    frsky->set_vario_request_callback(get_vario_data);
  }
  frsky->set_gps_request_callback(get_gps_data);
  frsky->set_rpm_request_callback(get_rpm_data);
  frsky->set_sp2uh_request_callback(get_sp2uh_data);
}

void DataBroker::write_factory_settings() {
  data_map.write_factory_settings();
  EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);
}

void DataBroker::console_map(char* p) {
  data_map.console_map(p);
}

void DataBroker::get_vario_data(int32_t *altitude, int32_t *vertical_speed) {
  *vertical_speed = (int32_t)(mav->climb_rate * 100);                                   
  *altitude = data_map.get_mapped_data(data_map.VARIO_ALTITUDE);
}

void DataBroker::get_fas_data(uint32_t *voltage, uint32_t *current) {
  *voltage = mav->average_battery_voltage;
  *current = mav->average_battery_current;
}
  
void DataBroker::get_gps_data(int32_t *lon, int32_t *lat, int32_t *alt, uint32_t *speed, uint32_t *heading) {
  *lon = mav->gps_longitude;
  *lat = mav->gps_latitude;
  *alt = mav->gps_altitude / 10;
  *speed = mav->gps_speed * 10;
  *heading = mav->heading * 100;            // 10000 = 100 deg   
}

void DataBroker::get_rpm_data(uint32_t *rpm) {
  *rpm = extended_message_processor.telem_next_extension_word();
  //*rpm = telem_next_extension_word();   
}

void DataBroker::get_sp2uh_data(uint32_t *fuel) {
  *fuel = mav->battery_remaining; 
}




