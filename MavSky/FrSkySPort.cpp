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
#include "FrSkySPort.h"
#include "MavLinkData.h"
#include "Logger.h"
#include "MavConsole.h"      // todo probably not needed

extern Logger *logger;
extern MavConsole *console;    // todo probably not needed

#define frsky_port                     Serial1
#define frsky_port_C1                  UART0_C1
#define frsky_port_C3                  UART0_C3
#define frsky_port_S2                  UART0_S2             

#define EXPIRY_MILLIS_FRSKY_REQUEST   1200
#define EXPIRY_MILLIS_FRSKY_VARIO     1200
#define EXPIRY_MILLIS_FRSKY_FAS       1200
#define EXPIRY_MILLIS_FRSKY_GPS       1200
#define EXPIRY_MILLIS_FRSKY_RPM       1200
#define EXPIRY_MILLIS_FRSKY_OTHER     1200

#define   DELAY_VARIO_PERIOD           500
#define   DELAY_FAS_PERIOD             500
#define   DELAY_GPS_PERIOD             500
#define   DELAY_RPM_PERIOD             100
#define   DELAY_SP2UH_PERIOD           500

FrSkySPort::FrSkySPort() {
  frsky_port.begin(57600);
  frsky_port_C3 = 0x10;            // TX invert
  frsky_port_C1 = 0xA0;            // Single wire
  frsky_port_S2 = 0x10;            // RX invert
}

int FrSkySPort::frsky_online() {  
  if(logger->get_timestamp_age(Logger::TIMESTAMP_FRSKY_REQUEST) < EXPIRY_MILLIS_FRSKY_REQUEST) {
    return 1;
  } else {
    return 0;
  }
}

void FrSkySPort::frsky_process(void) {
  uint8_t data = 0;

  while (frsky_port.available()) {
    data = frsky_port.read();
    logger->add_timestamp(Logger::TIMESTAMP_FRSKY_REQUEST);
    if(data == START_STOP) {
      waitingForSensorId = true; 
      continue; 
    }
    if(!waitingForSensorId) {
      continue;
    }
    frsky_process_sensor_request(data);
    waitingForSensorId = false;
  }
}

void FrSkySPort::set_vario_request_callback(void (*callback)(int32_t *p1, int32_t *p2)) {
  vario_data_request_function = callback;
}

void FrSkySPort::set_fas_request_callback(void (*callback)(uint32_t *p1, uint32_t *p2)) {
  fas_data_request_function = callback;
}

void FrSkySPort::set_gps_request_callback(void (*callback)(int32_t *p1, int32_t *p2, int32_t *p3, uint32_t *p4, uint32_t *p5)) {
  gps_data_request_function = callback;
}

void FrSkySPort::set_rpm_request_callback(void (*callback)(uint32_t *rpm)) {
  rpm_data_request_function = callback;
};

void FrSkySPort::set_sp2uh_request_callback(void (*callback)(uint32_t *fuel)) {
  sp2uh_data_request_function = callback;
};

void FrSkySPort::frsky_process_sensor_request(uint8_t sensorId) {
  static uint32_t delay_vario_next = 0;
  static uint32_t delay_fas_next = 0;
  static uint32_t delay_rpm_next = 0;
  static uint32_t delay_gps_next = 0;
  static uint32_t delay_sp2uh_next = 0;
  
  uint32_t latlong = 0;
  
  uint32_t process_timestamp = millis();
  switch(sensorId) {
    case SENSOR_ID_VARIO:
      logger->add_timestamp(Logger::TIMESTAMP_FRSKY_VARIO);
      switch(vario_sensor_state) {
        case 0:
          if(process_timestamp > delay_vario_next) {
            if(vario_data_request_function != NULL) {
              vario_data_request_function(&vario_altitude, &vario_vertical_speed);
            }
            frsky_send_package(FR_ID_ALTITUDE, vario_altitude);   
            delay_vario_next = process_timestamp + DELAY_VARIO_PERIOD;
            vario_sensor_state = 1;
          } else {
            frsky_send_null(FR_ID_ALTITUDE);   
          }
          break;
        case 1: 
          if(process_timestamp > delay_vario_next) {
            frsky_send_package(FR_ID_VARIO, vario_vertical_speed);   
            delay_vario_next = process_timestamp + DELAY_VARIO_PERIOD;
            vario_sensor_state = 0;
          } else {
            frsky_send_null(FR_ID_VARIO);   
          }
          break;
      }
      break;
      
    case SENSOR_ID_FAS:
      logger->add_timestamp(Logger::TIMESTAMP_FRSKY_FAS);
      switch(fas_sensor_state) {
        case 0:
          if(process_timestamp > delay_fas_next) {
            if(fas_data_request_function != NULL) {
              fas_data_request_function(&fas_voltage, &fas_current);
            }
            frsky_send_package(FR_ID_VFAS, fas_voltage);     
            delay_fas_next = process_timestamp + DELAY_FAS_PERIOD;   
            fas_sensor_state = 1;
          } else {
            frsky_send_null(FR_ID_VFAS);   
          }
          break;
        case 1:
          if(process_timestamp > delay_fas_next) {
            frsky_send_package(FR_ID_CURRENT, fas_current);
            delay_fas_next = process_timestamp + DELAY_FAS_PERIOD;   
            fas_sensor_state = 0;
          } else {
            frsky_send_null(FR_ID_CURRENT);   
          }
          break;
      }
      break;
  
    case SENSOR_ID_GPS:
      logger->add_timestamp(Logger::TIMESTAMP_FRSKY_GPS);     
      switch(gps_sensor_state)  {
        case 0:                                                
          if(process_timestamp > delay_gps_next) {
            if(gps_data_request_function != NULL) {
              gps_data_request_function(&gps_longitude, &gps_latitude, &gps_altitude, &gps_speed, &gps_heading);
            }        
            if(gps_longitude < 0) {
              latlong = (((0L - gps_longitude) / 100) * 6) | 0xC0000000;  // set msb to indicate lon, 2nd msb to indicate negative
            } else {
              latlong = ((gps_longitude / 100) * 6) | 0x80000000;         // set msb to indicate lon
            }
            frsky_send_package(FR_ID_LATLONG, latlong);
            delay_gps_next = process_timestamp + DELAY_GPS_PERIOD;   
            gps_sensor_state = 1;
          } else {
            frsky_send_null(FR_ID_LATLONG);   
          }
          break;
          
        case 1: 
          if(process_timestamp > delay_gps_next) {
            if(gps_latitude < 0L) {
              latlong = (((0L - gps_latitude) / 100) * 6) | 0x40000000;   // set 2nd msb to indicate negative
            } else {
              latlong = ((gps_latitude / 100) * 6);
            }
            frsky_send_package(FR_ID_LATLONG, latlong);
            delay_gps_next = process_timestamp + DELAY_GPS_PERIOD;
            gps_sensor_state = 2;
          } else {
            frsky_send_null(FR_ID_LATLONG);   
          }
          break;
            
        case 2:
          if(process_timestamp > delay_gps_next) {
            frsky_send_package(FR_ID_GPS_ALT, gps_altitude);                                 
            delay_gps_next = process_timestamp + DELAY_GPS_PERIOD;   
            gps_sensor_state = 3;
          } else {
            frsky_send_null(FR_ID_GPS_ALT);   
          }
          break;
          
        case 3:                 
          if(process_timestamp > delay_gps_next) {
            frsky_send_package(FR_ID_SPEED, gps_speed);                              
            delay_gps_next += DELAY_GPS_PERIOD;
            gps_sensor_state = 4;
          } else {
            frsky_send_null(FR_ID_SPEED);   
          }
          break;
          
        case 4:
          if(process_timestamp > delay_gps_next) {
            frsky_send_package(FR_ID_GPS_COURSE, gps_heading);                                      
            delay_gps_next = process_timestamp + DELAY_GPS_PERIOD;   
            gps_sensor_state = 0;
          } else {
            frsky_send_null(FR_ID_GPS_COURSE);   
          }
          break;
      }
      break;   
       
    case SENSOR_ID_RPM:
      logger->add_timestamp(Logger::TIMESTAMP_FRSKY_RPM);
      if(process_timestamp > delay_rpm_next) {
        if(rpm_data_request_function != NULL) {
          rpm_data_request_function(&rpm);
        }           
        frsky_send_package(FR_ID_RPM, rpm);
        delay_rpm_next = process_timestamp + DELAY_RPM_PERIOD;   
      } else {
        frsky_send_null(FR_ID_RPM);   
      }
      break;

    case SENSOR_ID_SP2UH:
      logger->add_timestamp(Logger::TIMESTAMP_FRSKY_OTHER); 
      if(process_timestamp > delay_sp2uh_next) {
        if(sp2uh_data_request_function != NULL) {
          sp2uh_data_request_function(&sp2uh_fuel);
        }    
        frsky_send_package(FR_ID_FUEL, sp2uh_fuel); 
        delay_sp2uh_next = process_timestamp + DELAY_SP2UH_PERIOD;   
      } else {
        frsky_send_null(FR_ID_FUEL);   
      }   
      break;

    case SENSOR_ID_SP2UR:
      break;  
      
    case SENSOR_ID_FLVSS:
      break;           
  }
}

void FrSkySPort::frsky_send_byte(uint8_t byte) {
  if(byte == 0x7E) {
    frsky_port.write(0x7D);
    frsky_port.write(0x5E);
  } else if(byte == 0x7D) {
    frsky_port.write(0x7D);
    frsky_port.write(0x5D);
  } else {
    frsky_port.write(byte);
  }
  frsky_update_crc(byte);
}

void FrSkySPort::frsky_update_crc(uint8_t byte) {
  crc += byte;           //0-1FF
  crc += crc >> 8;       //0-100
  crc &= 0x00ff;
}

void FrSkySPort::frsky_send_crc() {
  frsky_port.write(0xFF - crc);
  crc = 0;       
}

void FrSkySPort::frsky_send_package(uint16_t id, uint32_t value) {
  frsky_port_C3 |= 32;                      //  TX
  frsky_send_byte(DATA_FRAME);
  uint8_t *bytes = (uint8_t*)&id;
  frsky_send_byte(bytes[0]);
  frsky_send_byte(bytes[1]);
  bytes = (uint8_t*)&value;
  frsky_send_byte(bytes[0]);
  frsky_send_byte(bytes[1]);
  frsky_send_byte(bytes[2]);
  frsky_send_byte(bytes[3]);
  frsky_send_crc();
  frsky_port.flush();
  frsky_port_C3 ^= 32;                      // RX
}

void FrSkySPort::frsky_send_null(uint16_t id)
{
  frsky_port_C3 |= 32;                      //  TX
  frsky_send_byte(0x00);
  uint8_t *b = (uint8_t*)&id;
  frsky_send_byte(b[0]);
  frsky_send_byte(b[1]);
  frsky_send_byte(0x00);
  frsky_send_byte(0x00);
  frsky_send_byte(0x00);
  frsky_send_byte(0x00);  
  frsky_send_crc();
  frsky_port.flush();
  frsky_port_C3 ^= 32;                      // RX
}
