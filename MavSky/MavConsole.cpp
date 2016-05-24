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
#include "MavConsole.h"

#include "../EEPROM/EEPROM.h"

#include <string.h>
#include "MavSky.h"
#include "Logger.h"
#include "DataBroker.h"       // todo needed for eeprom constants for now
#include "Led.h"

extern Logger *logger;
extern MavLinkData *mav;
extern DataBroker data_broker;
extern LedController* led_strip_ptr;
extern uint8_t led_map[];

uint8_t led_code_buffer[EEPROM_LED_CODE_MAX_SIZE];
uint16_t led_code_size = 0;
 
uint16_t get_crc(uint8_t* data_p, uint16_t len){
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while (len--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}

MavConsole::MavConsole(usb_serial_class port) {
  serial = port;
  serial.begin(57000);
}

MavConsole::~MavConsole() {  
}

void MavConsole::console_print(const char* fmt, ...) {
    char formatted_string[256];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(formatted_string, fmt, argptr);
    va_end(argptr);
    serial.print(formatted_string);
}

void MavConsole::do_help() {
  console_print("%s\r\n", PRODUCT_STRING);
  console_print("debug mav [all|heartbeat|gps|attitude|imu|vfr|status|text|rangefinder|rc|other] [on|off]\r\n"); 
  console_print("debug frsky [all|vario|fcs|rpm] [on|off]\r\n"); 
  console_print("debug temp [on|off]\r\n"); 
  console_print("dump\r\n");
  console_print("timing\r\n");
  console_print("map\r\n");
  console_print("map [bar_altitude|rangefinder_distance] vario_altitude [scale]\r\n");                                    
  console_print("map [climb_rate] vario_vertical_speed [scale]\r\n");                                    
  console_print("frsky [vfas|vario] [enable|disable]\r\n");
  console_print("ledmap L1 R1 L2 R2 L3 R3 L4 R4\r\n");
  console_print("factory\r\n");
}

char const *MavConsole::on_off(int v) {
  if(v) {
    return "on";
  } else {
    return "off";
  }
}
  
void MavConsole::parse_debug_on_off(char* p, int *debug_var_ptr, char *name) {
  if(strcmp(p, "on") == 0) {
    *debug_var_ptr = 1;
    console_print(" on\r\n");
  } else if(strcmp(p, "off") == 0) {
    *debug_var_ptr = 0;
    console_print(" off\r\n");
  } else if(p == NULL) {
    console_print("%s\r\n", on_off(*debug_var_ptr));
  } else {
    console_print("Unknown parameter\r\n");
  }
}  

void MavConsole::do_dump() {
  console_print("Battery voltage:           %.2f\r\n", mav->battery_voltage / 1000.0);  
  console_print("Battery current:           %.2f\r\n", mav->battery_current / 100.0);
  console_print("Battery percent remaining: %d\r\n", mav->battery_remaining);
  console_print("HDOP:                      %.2f\r\n", mav->gps_hdop / 100.0);
  console_print("Satellites visible:        %d\r\n", mav->gps_satellites_visible);
  console_print("Temperature:               %d\r\n", mav->temperature);
  console_print("Barometric Altitude:       %f\r\n", mav->bar_altitude);    
  console_print("RangeFinder Altitude:      %d\r\n", mav->rangefinder_distance);      
  console_print("Temperature:               %d\r\n", mav->temperature);
  console_print("Mavlink imu:               x:%-4d y:%-4d z:%-4d\r\n", mav->imu_xacc, mav->imu_yacc, mav->imu_zacc);
  console_print("Summed mah consumed:       %d\r\n", mav->calc_mah_consumed());
  console_print("rc7:                       %d\r\n", mav->rc7);
  console_print("rc8:                       %d\r\n", mav->rc8);

  //led_strip_ptr->dump_diags();
}

void MavConsole::do_times() {
  console_print("Mavlink heartbeat:         %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT));
  console_print("Mavlink statustext:        %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT)); 
  console_print("Mavlink sys status:        %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS));
  console_print("Mavlink gps:               %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT));
  console_print("Mavlink raw imu:           %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU));
  console_print("Mavlink hud:               %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD)); 
  console_print("Mavlink attitude:          %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE));
  console_print("Mavlink mission current:   %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT));
  console_print("Mavlink scaled pressure:   %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE));
  console_print("Mavlink controller output: %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT));
  console_print("Mavlink rangefinder:       %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER)); 
  console_print("Mavlink rc raw:            %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_MAVLINK_MSG_ID_RC_CHANNELS_RAW)); 

  console_print("FrSky vario:               %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_FRSKY_VARIO)); 
  console_print("FrSky fas:                 %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_FRSKY_FAS)); 
  console_print("FrSky gps:                 %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_FRSKY_GPS)); 
  console_print("FrSky rpm:                 %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_FRSKY_RPM)); 
  console_print("FrSky other:               %d\r\n", logger->get_timestamp_delta(Logger::TIMESTAMP_FRSKY_OTHER)); 
}

void MavConsole::do_map(char* p) {
  data_broker.console_map(p);
}

void MavConsole::do_frsky() {
  char* p;
  
  p = strtok(NULL, " ");
  if(strcmp(p, "vfas") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "enable") == 0) {
        EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);
      } else if(strcmp(p, "disable") == 0) {
        EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 0);
      }

    }
    if(EEPROM.read(EEPROM_ADDR_FRSKY_VFAS_ENABLE)) {
      console_print("frsky vfas enabled\r\n");
    } else {
      console_print("frsky vfas disabled\r\n");
    }
  } else if(strcmp(p, "vario") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "enable") == 0) {
        EEPROM.write(EEPROM_ADDR_FRSKY_VARIO_ENABLE, 1);
      } else if(strcmp(p, "disable") == 0) {
        EEPROM.write(EEPROM_ADDR_FRSKY_VARIO_ENABLE, 0);
      }
    }
    if(EEPROM.read(EEPROM_ADDR_FRSKY_VARIO_ENABLE)) {
      console_print("frsky vario enabled\r\n");
    } else {
      console_print("frsky vario disabled\r\n");
    }
  }
}

void MavConsole::do_ldump() {
  for(uint16_t i=0; i<16; i++) {
    console_print("%02x ", EEPROM.read(EEPROM_LED_CODE_BASE + i));
  }  
  console_print("\r\n");
}
     
void MavConsole::do_factory() {
  data_broker.write_factory_settings();
  console_print("Settings restored to factory values.  Please reboot\r\n");
}

void MavConsole::do_led_map() {
  char* p;
  uint8_t temp_map[8];

  for(int i=0; i<8; i++) {
    temp_map[i] = 9;
  }
  for(int i=0; i<8; i++) {
    p = strtok(NULL, " ");
    if(strlen(p) == 0) {
       console_print("Please specify 8 arguments to the ledmap command\r\n");
       return;
    }
    uint8_t observed_led_number = (uint8_t)atoi(p);
    if(observed_led_number < 1 || observed_led_number > 8) {
       console_print("LED numbers must be between 1 and 8\r\n");
       return;
    }
    uint8_t internal_led_number = observed_led_number - 1;
    for(int i=0; i<8; i++) {
      if(temp_map[i] == internal_led_number) {
        console_print("LED number %d has been specified more than once.  Please retry\r\n", observed_led_number);
        return;    
      }      
    }
    temp_map[i] = internal_led_number;
  }
  for(int i=0; i<8; i++) {
    EEPROM.write(EEPROM_LED_MAP_BASE + i, temp_map[i]);
  }
  led_strip_ptr->reload();
  console_print("The new LED map has been written to NVRAM and is now being used\r\n");
}

void MavConsole::do_command(char *cmd_buffer) {
  char* p;
  
  p = strtok(cmd_buffer, " ");
  if(p != NULL) {
    if(strcmp(p, "debug") == 0) {
      p = strtok(NULL, " ");
      if(strcmp(p, "mav") == 0) {
        p = strtok(NULL, " ");
        if(strcmp(p, "all") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavAllEnable), (char *)"Mav All");
        } else if (strcmp(p, "heartbeat") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavHeartbeatEnable), (char *)"Mav Heartbeat");
        } else if (strcmp(p, "gps") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavGpsEnable), (char *)"Mav GPS");
        } else if (strcmp(p, "attitude") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavAttitudeEnable), (char *)"Mav Attitude");
        } else if (strcmp(p, "imu") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavImuEnable), (char *)"Mav IMU");            
        } else if (strcmp(p, "vfr") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavVfrEnable), (char *)"Mav VFR");
        } else if (strcmp(p, "status") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavStatusEnable), (char *)"Mav Status");
        } else if (strcmp(p, "text") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavTextEnable), (char *)"Mav Text");
        } else if (strcmp(p, "other") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavOtherEnable), (char *)"Mav Other");
        } else if (strcmp(p, "rangefinder") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavRangeFinderEnable), (char *)"Mav RangeFinder");
        } else if (strcmp(p, "rc") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugMavRcChannelsRawEnable), (char *)"Mav RC");
        } else {
          console_print("Unknown parameter %s\r\n", p);
        }
      } else if(strcmp(p, "frsky") == 0) {
        p = strtok(NULL, " ");
        if(strcmp(p, "all") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugFrskyAllEnable), (char *)"FrSky All");
        } else if (strcmp(p, "rpm") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugFrskyRpmEnable), (char *)"FrSky RPM");
        } else if (strcmp(p, "vario") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugFrskyVarioEnable), (char *)"FrSky Vario");
        } else if (strcmp(p, "fas") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &(logger->debugFrskyFasEnable), (char *)"FrSky Fas");
        } 
      } else if(strcmp(p, "temp") == 0) {
        p = strtok(NULL, " ");
        parse_debug_on_off(p, &(logger->debugTempEnable), (char *)"Temp");
      }
    } else if(strcmp(p, "dump") == 0) {
      do_dump();
    } else if(strcmp(p, "timing") == 0) {
      do_times();
    } else if(strcmp(p, "map") == 0) {
      do_map(p);
    } else if(strcmp(p, "frsky") == 0) {
       do_frsky();  
    } else if(strcmp(p, "ldump") == 0) {
      do_ldump();
    } else if(strcmp(p, "ledmap") == 0) {
      do_led_map();
    } else if(strcmp(p, "factory") == 0) {
      do_factory();
    } else if(strcmp(p, "help") == 0) {
      do_help();
    } else {
      console_print("Unknown command\r\n");
    }
  }
}

uint8_t MavConsole::atoh(uint8_t c)
{
  uint8_t result = 0;

  if (c >= '0' && c <= '9') {
      result = (c - '0');
  } else if (c >= 'a' && c <= 'f') {
      result = (c - 'a') + 10;
  } else if (c >= 'A' && c <= 'F') {
      result = (c - 'A') + 10;
  }
  return result;
}

void MavConsole::check_for_console_command() {
  while(serial.available()) { 
    uint8_t c = serial.read();
    if(c == '\r' || c == '\n') {
      cmd_buffer[cmd_index++] = '\0';
      cmd_index = 0;      
      if(led_data_mode) {
        if(strcmp(cmd_buffer, "datastop") == 0) {
          console_print("\r\ndatastop\r\n");            
          if(led_code_size > 2) {
            uint16_t download_crc = (led_code_buffer[led_code_size-2] << 8) + led_code_buffer[led_code_size - 1];         
            uint16_t calculated_crc = get_crc(led_code_buffer, led_code_size - 2);
            if(download_crc != calculated_crc) {
              console_print("CRC error.  Calculated CRC is %04x but should have been %04x\r\n", calculated_crc, download_crc);
            } else {   
              EEPROM.write(EEPROM_LED_CODE_SIZE, (led_code_size >> 8) & 0xff);
              EEPROM.write(EEPROM_LED_CODE_SIZE + 1, led_code_size & 0xff);              
              for(uint16_t i=0; i<led_code_size; i++) {
                EEPROM.write(EEPROM_LED_CODE_BASE + i, led_code_buffer[i]);
              }  
              led_strip_ptr->reload();
              console_print("Data successfully written to NVRAM.  LED patterns have been reloaded.\r\n");
            }
          } else {
            console_print("Insufficient data received\r\n");            
          } 
          led_data_mode = 0;
          cmd_index = 0;
          serial.write("]"); 
        } else {
          while((uint16_t)cmd_index < strlen(cmd_buffer)) {
            uint8_t high = atoh(cmd_buffer[cmd_index++]);
            uint8_t low = atoh(cmd_buffer[cmd_index++]);
            uint16_t data = (high << 4) + low;
            if(led_code_size < EEPROM_LED_CODE_MAX_SIZE) {
              led_code_buffer[led_code_size++] = data;
            }
          }       
          cmd_index = 0;
          console_print(".");            

        }
      } else {
        serial.write("\r\n");       
        if(strcmp(cmd_buffer, "datastart") == 0) {
          led_data_mode = 1;
          led_code_size = 0;
          cmd_index = 0;         
        } else {
          do_command(cmd_buffer);
          serial.write("]");
        }
      }
    } else {
      if(!led_data_mode) {
        serial.write(c);
      }
      cmd_buffer[cmd_index++] = tolower(c);
    }
  }
}
