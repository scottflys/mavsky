
// a1                                           // dont know how to set this one 
// a2             <- gps_hdop / 4
// a3             <- roll
// a4             <- pitch   
// accx           <- average_xacc, xacc         //                               *configurable
// accy           <- average_yacc, yacc         //                               *configurable
// accz           <- average_zacc, zacc         //                               *configurable
// air-speed      <-                            // todo - dont know how to set this
// altitude       <- bar_altitude               // 100 = 1m     from barometer
// current        <- average_current, current   // 10  = 10ma                    *configurable
// fuel           <- custom_mode                //
// gps-altitude   <- gps_altitude               // 100 = 1m     from gps
// gps-speed      <- gps_speed                  // 100 = 1m/s   setting gps_speed sets air-speed too           
// heading        <- gps_heading                // 10000 = 100 degrees         
// latitude       <- gps_latitude
// longitude      <- gps_longitude
// rpm            <- used for text message
// temp1          <- gps_sats_visible * 10 + gps_fixtype
// temp2          <- battery_remaining, mission_current_seq, temperature, wp_dist  // *configurable
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
uint8_t gps_first_position_good = 0;

// Scale factor for roll/pitch:
// We need to scale down 360 deg to fit when max value is 256, and 256 equals 362 deg
float scalefactor = 360.0/((362.0/360.0)*256.0);

void frsky_init(void)  {
  frsky_port.begin(FRSKY_BAUD);
  frsky_port_C3 = 0x10;            // Tx invert
  frsky_port_C1 = 0xA0;            // Single wire mode
  frsky_port_S2 = 0x10;            // Rx Invert
  frsky_send_text_message("Initializing...");
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
  uint8_t hdop_threshold;

  if(!gps_first_position_good) {
    if(mav.gps_fixtype == 3 && mav.gps_hdop <= (EEPROM.read(EEPROM_ADDR_HDOP_THRESHOLD) * 100)) {
      gps_first_position_good = 1;
    }
  }
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
          frsky_send_package(FR_ID_ALTITUDE, mav.bar_altitude);   // from barometer, 100 = 1m
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
          frsky_send_package(FR_ID_VFAS, telem_data_get_value(TELEM_DATA_VFAS) / 10);       // Sends voltage as a VFAS value
          break;
        case 1:
          frsky_send_package(FR_ID_CURRENT, telem_data_get_value(TELEM_DATA_CURRENT) / 10);
          break;
      }
      if(++next_fas > 1)
        next_fas = 0;
      break;
  
    case SENSOR_ID_GPS:
      add_timestamp(TIMESTAMP_FRSKY_GPS);
      if(!mavlink_gps_data_valid() || !gps_first_position_good) {
        break;
      }
      switch(next_gps)  {
        case 0:                 // Sends the longitude value, setting bit 31 high
          if(mav.gps_longitude < 0) {
            latlong=((abs(mav.gps_longitude)/100)*6)  | 0xC0000000;
          } else {
            latlong=((abs(mav.gps_longitude)/100)*6)  | 0x80000000;
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
          frsky_send_package(FR_ID_GPS_ALT, mav.gps_altitude / 10);        // from GPS,  100=1m
          break;
        case 3:
          //frsky_send_package(FR_ID_SPEED, mav.gps_speed * 36);             // shows up on taranis as km/hr             
          frsky_send_package(FR_ID_SPEED, telem_data_get_value(TELEM_DATA_GPS_SPEED));                   // shows up on taranis as km/hr             
          break;
        case 4:
          frsky_send_package(FR_ID_GPS_COURSE, mav.heading * 100);         // 10000 = 100 deg            // todo - this isn't GPS, but using mavlink_gps_data_valid() to check validity of this whole section
          break;
      }
      if(++next_gps > 4) {
        next_gps = 0;
      }
      break;    
  
    case SENSOR_ID_RPM:
      add_timestamp(TIMESTAMP_FRSKY_RPM);
      uint16_t data_word;
      data_word = telem_text_get_word();
      debug_print(LOG_FRSKY_RPM, "FRSKY SENSOR_ID_RPM: %d", data_word);            
      frsky_send_package(FR_ID_RPM, data_word);   
      break;
  
    case 0x45:
    case 0xC6:
      add_timestamp(TIMESTAMP_FRSKY_OTHER);
      int32_t gps_status;
      switch(next_default) {
        case 0:      
          int16_t hdop_val;
          hdop_val = mav.gps_hdop / 4;
          if(hdop_val > 249) {
            hdop_val = 249;
          }
          frsky_send_package(FR_ID_ADC2, hdop_val);                         //  value must be between 0 and 255.  1 produces 0.4          
          break;       
        case 1:
          frsky_send_package(FR_ID_ACCX, telem_data_get_value(TELEM_DATA_ACCX));    
          break;
        case 2:
          frsky_send_package(FR_ID_ACCY, telem_data_get_value(TELEM_DATA_ACCY)); 
          break; 
        case 3:
          frsky_send_package(FR_ID_ACCZ, telem_data_get_value(TELEM_DATA_ACCZ)); 
          break; 
        case 4:
          gps_status = (mav.gps_satellites_visible*10) + mav.gps_fixtype;
          frsky_send_package(FR_ID_T1, gps_status); 
          break; 
        case 5:
          frsky_send_package(FR_ID_A3_FIRST, handle_A2_A3_value((mav.roll_angle+180)/scalefactor));
          break;
        case 6:
          frsky_send_package(FR_ID_A4_FIRST, handle_A2_A3_value((mav.pitch_angle+180)/scalefactor));
          break;
        case 7:
          frsky_send_package(FR_ID_T2, telem_data_get_value(TELEM_DATA_T2));                    
          break;
        case 8:
          frsky_send_package(FR_ID_FUEL, mav.custom_mode);   
          break;      
      }
      if(++next_default > 8) {
        next_default = 0;
      }
      break;
  }
}

uint32_t handle_A2_A3_value(uint32_t value)
{
  return (value *330-165)/0xFF;
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
