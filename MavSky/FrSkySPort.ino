
// a1                                       
// a2            
// a3             
// a4         
// accx           <- average_xacc, xacc         //                               *configurable
// accy           <- average_yacc, yacc         //                               *configurable
// accz           <- average_zacc, zacc         //                               *configurable
// air-speed      <-                            // todo - dont know how to set this
// altitude       <- bar_altitude               // 100 = 1m                      *configurable    2015-08-23
// current        <- average_current, current   // 10  = 10ma                    *configurable
// fuel           <- custom_mode                //
// gps-altitude   <- gps_altitude               // 100 = 1m     from gps
// gps-speed      <- gps_speed                  // 100 = 1m/s   setting gps_speed sets air-speed too           
// heading        <- gps_heading                // 10000 = 100 degrees         
// latitude       <- gps_latitude
// longitude      <- gps_longitude
// rpm            <- used for text message
// temp1          <- gps_sats_visible * 10 + gps_fixtype
// temp2          <- battery_remaining, mission_current_seq, temperature, wp_dist, hdop, armed // *configurable
// vertical-speed <- ap_climb_rate              // 100 = 1m/s
// vfas           <- average_voltage, voltage   // 10  = 1mv                     *configurable

#include "FrSkySPort.h"
#include "TelemetryData.h"

#define frsky_port                     Serial1
#define frsky_port_C1                  UART0_C1
#define frsky_port_C3                  UART0_C3
#define frsky_port_S2                  UART0_S2

#define FRSKY_BAUD                    57600

#define EXPIRY_MILLIS_FRSKY_REQUEST   1200
#define EXPIRY_MILLIS_FRSKY_VARIO     1200
#define EXPIRY_MILLIS_FRSKY_FAS       1200
#define EXPIRY_MILLIS_FRSKY_GPS       1200
#define EXPIRY_MILLIS_FRSKY_RPM       1200
#define EXPIRY_MILLIS_FRSKY_OTHER     1200

short crc;                      
boolean waitingForSensorId = false;
uint8_t next_fas = 0;
uint8_t next_vario = 0;
uint8_t next_gps = 0;
uint8_t next_default = 0;

// Scale factor for roll/pitch:
// We need to scale down 360 deg to fit when max value is 256, and 256 equals 362 deg
float scalefactor = 360.0/((362.0/360.0)*256.0);

void frsky_init(void)  {
  frsky_port.begin(FRSKY_BAUD);
  frsky_port_C3 = 0x10;            // Tx invert
  frsky_port_C1 = 0xA0;            // Single wire mode
  frsky_port_S2 = 0x10;            // Rx Invert
}

int frsky_online() {  
  if(get_timestamp_age(TIMESTAMP_FRSKY_REQUEST) < EXPIRY_MILLIS_FRSKY_REQUEST) {
    return 1;
  } else {
    return 0;
  }
}

void frsky_process(void) {
  uint8_t data = 0;

  while (frsky_port.available()) {
    data = frsky_port.read();
    add_timestamp(TIMESTAMP_FRSKY_REQUEST);
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

void frsky_process_sensor_request(uint8_t sensorId) {
  uint32_t latlong = 0;

  switch(sensorId) {
    case SENSOR_ID_VARIO:
      add_timestamp(TIMESTAMP_FRSKY_VARIO);
      if(!mavlink_vfr_data_valid()) {
        break;
      }
      switch(next_vario) {
        case 0:
          frsky_send_package(FR_ID_VARIO, mav.ap_climb_rate);       // 100 = 1m/s  
          break;
        case 1: 
          int32_t alt_val = telem_data_get_value(TELEM_DATA_ALT);
          frsky_send_package(FR_ID_ALTITUDE, alt_val);   //from barometer or rangefinder, 100 = 1m
          break;
      }
      if(++next_vario > 1) {
        next_vario = 0;
      }
      break;
      
    case SENSOR_ID_FAS:
      add_timestamp(TIMESTAMP_FRSKY_FAS);
      if(!mavlink_sys_status_data_valid() || !EEPROM.read(EEPROM_ADDR_FRSKY_VFAS_ENABLE)) {
        break;
      }
      switch(next_fas) {
        case 0:
          uint16_t volt_val;
          volt_val = telem_data_get_value(TELEM_DATA_VFAS) / 10;
          frsky_send_package(FR_ID_VFAS, volt_val);       // Sends voltage as a VFAS value
          break;
        case 1:
          int16_t curr_val = telem_data_get_value(TELEM_DATA_CURRENT) / 10;
          frsky_send_package(FR_ID_CURRENT, curr_val);
          break;
      }
      if(++next_fas > 1)
        next_fas = 0;
      break;
  
    case SENSOR_ID_GPS:
      add_timestamp(TIMESTAMP_FRSKY_GPS);
      if(!mavlink_gps_data_valid()) {
        break;
      }
      switch(next_gps)  {
        case 0:                 // Sends the longitude value, setting bit 31 high
          if(mav.gps_longitude < 0) {
            latlong=((abs(mav.gps_longitude)/100)*6) | 0xC0000000;
          } else {
            latlong=((abs(mav.gps_longitude)/100)*6) | 0x80000000;
          }
          frsky_send_package(FR_ID_LATLONG, latlong);
          break;
        case 1: 
          if(mav.gps_latitude < 0 ) {
            latlong = ((abs(mav.gps_latitude)/100)*6) | 0x40000000;
          } else {
            latlong = ((abs(mav.gps_latitude)/100)*6);
          }
          frsky_send_package(FR_ID_LATLONG, latlong);
          break;  
        case 2:
          frsky_send_package(FR_ID_GPS_ALT, mav.gps_altitude / 10);                                     // from GPS,  100=1m
          break;
        case 3:          
          frsky_send_package(FR_ID_SPEED, telem_data_get_value(TELEM_DATA_GPS_SPEED));                  // shows up on taranis as km/hr             
          break;
        case 4:
          frsky_send_package(FR_ID_GPS_COURSE, mav.heading * 100);                                      // 10000 = 100 deg   
          break;
      }
      if(++next_gps > 4) {
        next_gps = 0;
      }
      break;    
  
    case SENSOR_ID_RPM:
      add_timestamp(TIMESTAMP_FRSKY_RPM);
      uint16_t data_word;
      data_word = telem_next_extension_word();
      debug_print(LOG_FRSKY_RPM, "FRSKY SENSOR_ID_RPM: %d", data_word);            
      frsky_send_package(FR_ID_RPM, data_word);   
      break;

    case SENSOR_ID_SP2UH:
      add_timestamp(TIMESTAMP_FRSKY_OTHER);
      switch(next_default) {
        case 0:      
          frsky_send_package(FR_ID_FUEL, mav.battery_remaining); 
          break;               
      }
      if(++next_default > 1) {
        next_default = 0;
      }      
      break;
  }
}

void frsky_send_byte(uint8_t byte) {
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

void frsky_update_crc(uint8_t byte) {
  crc += byte;           //0-1FF
  crc += crc >> 8;       //0-100
  crc &= 0x00ff;
}

void frsky_send_crc() {
  frsky_port.write(0xFF - crc);
  crc = 0;       
}

void frsky_send_package(uint16_t id, uint32_t value) {
  frsky_port_C3 |= 32;                      //  Transmit direction, to S.Port
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
  frsky_port_C3 ^= 32;                      // Transmit direction, from S.Port
}
