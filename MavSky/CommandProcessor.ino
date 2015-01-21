#include <EEPROM.h>
#include <STRING.h>
#include "Logger.h"
#include "TelemetryData.h"

char cmd_buffer[32];
int cmd_index = 0;

void do_help() {
  console_print("%s\r\n", PRODUCT_STRING);
  console_print("debug mav [all|heartbeat|gps|attitude|imu|vfr|status|text|other] [on|off]\r\n"); 
  console_print("debug frsky [all|rpm] [on|off]\r\n"); 
  console_print("debug temp [on|off]\r\n"); 
  console_print("dump\r\n");
  console_print("timing\r\n");
  console_print("msg [MESSAGE TEXT]\r\n");
  console_print("map\r\n");
  console_print("map vfas     [direct|average10|average50]\r\n");
  console_print("map current  [direct|average10|average50]\r\n");
  console_print("map accx     [direct|average10|average50|peak10|peak50]\r\n");
  console_print("map accy     [direct|average10|average50|peak10|peak50]\r\n");
  console_print("map accz     [direct|average10|average50|peak10|peak50]\r\n");
  console_print("map gpsspeed [kph|mps]\r\n");
  console_print("map t2       [batt_remain|mission_seq|temp|wp_dist|hdop]\r\n");
  console_print("set hdop     [1-10]\r\n");
  console_print("frsky vfas   [enable|disable]\r\n");
  console_print("factory\r\n");
}

char *on_off(int v) {
  if(v) {
    return "on";
  } else {
    return "off";
  }
}
  
void parse_debug_on_off(char* p, int *debug_var_ptr, char *name) {
  int mode;
  
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

void do_dump() {
  int16_t imu_xacc_ave, imu_yacc_ave, imu_zacc_ave;

  console_print("Battery voltage:           %.2f\r\n", mav.battery_voltage / 1000.0);  
  console_print("Battery voltage average:   %.2f\r\n", mavlink_get_average(mav.battery_voltage_buffer, mav.battery_voltage_buffer_start, mav.battery_voltage_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE) / 1000.0);
  console_print("Battery current:           %.2f\r\n", mav.battery_current / 100.0);
  console_print("Battery current average:   %.2f\r\n", mavlink_get_average(mav.battery_current_buffer, mav.battery_current_buffer_start, mav.battery_current_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE) / 100.0);
  console_print("Battery percent remaining: %d\r\n", mav.battery_remaining);
  console_print("HDOP:                      %.2f\r\n", mav.gps_hdop / 100.0);
  console_print("Satellites visible:        %d\r\n", mav.gps_satellites_visible);
  console_print("Temperature:               %d\r\n", mav.temperature);
  console_print("Mavlink imu:               x:%-4d y:%-4d z:%-4d\r\n", mav.imu_xacc, mav.imu_yacc, mav.imu_zacc);
  
  imu_xacc_ave = mavlink_get_average(mav.imu_xacc_buffer, mav.imu_xacc_buffer_start, mav.imu_xacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  imu_yacc_ave = mavlink_get_average(mav.imu_yacc_buffer, mav.imu_yacc_buffer_start, mav.imu_yacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  imu_zacc_ave = mavlink_get_average(mav.imu_zacc_buffer, mav.imu_zacc_buffer_start, mav.imu_zacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  console_print("Mavlink imu average:       x:%-4d y:%-4d z:%-4d\r\n", imu_xacc_ave, imu_yacc_ave, imu_zacc_ave);
  
  imu_xacc_ave = mavlink_get_average(mav.imu_xacc_peak_buffer, mav.imu_xacc_peak_buffer_start, mav.imu_xacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  imu_yacc_ave = mavlink_get_average(mav.imu_yacc_peak_buffer, mav.imu_yacc_peak_buffer_start, mav.imu_yacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  imu_zacc_ave = mavlink_get_average(mav.imu_zacc_peak_buffer, mav.imu_zacc_peak_buffer_start, mav.imu_zacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
  console_print("Mavlink imu peak average:  x:%-4d y:%-4d z:%-4d\r\n", imu_xacc_ave, imu_yacc_ave, imu_zacc_ave);
}

void do_times() {
  console_print("Mavlink heartbeat:         %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT));
  console_print("Mavlink statustext:        %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT)); 
  console_print("Mavlink sys status:        %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS));
  console_print("Mavlink gps:               %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT));
  console_print("Mavlink raw imu:           %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU));
  console_print("Mavlink hud:               %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD)); 
  console_print("Mavlink attitude:          %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE));
  console_print("Mavlink mission current:   %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT));
  console_print("Mavlink scaled pressure:   %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE));
  console_print("Mavlink controller output: %d\r\n", get_timestamp_delta(TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT));
  console_print("FrSky vario:               %d\r\n", get_timestamp_delta(TIMESTAMP_FRSKY_VARIO)); 
  console_print("FrSky fas:                 %d\r\n", get_timestamp_delta(TIMESTAMP_FRSKY_FAS)); 
  console_print("FrSky gps:                 %d\r\n", get_timestamp_delta(TIMESTAMP_FRSKY_GPS)); 
  console_print("FrSky rpm:                 %d\r\n", get_timestamp_delta(TIMESTAMP_FRSKY_RPM)); 
  console_print("FrSky other:               %d\r\n", get_timestamp_delta(TIMESTAMP_FRSKY_OTHER)); 
}

void do_msg(char* message_text) {
  frsky_send_text_message(message_text);
}

void do_map_dump() {
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_VFAS)) {
    case EEPROM_VALUE_MAP_VFAS_DIRECT:
     console_print("voltage direct routed to vfas\r\n");
     break;
   case EEPROM_VALUE_MAP_VFAS_AVERAGE10:
     console_print("voltage average10 routed to vfas\r\n");
     break;
   case EEPROM_VALUE_MAP_VFAS_AVERAGE50:
     console_print("voltage average50 routed to vfas\r\n");
     break;
   default:
     console_print("Invalid data routed to vfas\r\n");
     break;    
  }  
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT)) {
    case EEPROM_VALUE_MAP_CURRENT_DIRECT:
     console_print("current direct routed to current\r\n");
     break;
   case EEPROM_VALUE_MAP_CURRENT_AVERAGE10:
     console_print("current average10 routed to current\r\n");
     break;
   case EEPROM_VALUE_MAP_CURRENT_AVERAGE50:
     console_print("current average50 routed to current\r\n");
     break;
   default:
     console_print("Invalid data routed to current\r\n");
     break;    
  }  
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCX)) {
    case EEPROM_VALUE_MAP_ACCX_DIRECT:
     console_print("accx direct routed to accx\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCX_AVERAGE10:
     console_print("accx average10 routed to accx\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCX_AVERAGE50:
     console_print("accx average50 routed to accx\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10:
     console_print("accx peak10 routed to accx\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE50:
     console_print("accx peak50 routed to accx\r\n");
     break;
   default:
     console_print("Invalid data routed to accx\r\n");
     break;    
  }  
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCY)) {
    case EEPROM_VALUE_MAP_ACCY_DIRECT:
     console_print("accy direct routed to accy\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCY_AVERAGE10:
     console_print("accy average10 routed to accy\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCY_AVERAGE50:
     console_print("accy average50 routed to accy\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10:
     console_print("accy peak10 routed to accy\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE50:
     console_print("accy peak50 routed to accy\r\n");
     break;     
   default:
     console_print("Invalid data routed to accy\r\n");
     break;    
  }      
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ)) {
    case EEPROM_VALUE_MAP_ACCZ_DIRECT:
     console_print("accz direct routed to accz\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCZ_AVERAGE10:
     console_print("accz average10 routed to accz\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCZ_AVERAGE50:
     console_print("accz average50 routed to accz\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10:
     console_print("accz peak10 routed to accz\r\n");
     break;
   case EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE50:
     console_print("accz peak50 routed to accz\r\n");
     break;
   default:
     console_print("Invalid data routed to accz\r\n");
     break;    
  }      
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED)) {
    case EEPROM_VALUE_MAP_GPS_SPEED_KPH:
     console_print("gpsspeed kph routed to gps-speed\r\n");
     break;
   case EEPROM_VALUE_MAP_GPS_SPEED_MPS:
     console_print("gpsspeed mps routed to gps-speed\r\n");
     break;
   default:
     console_print("Invalid data routed to gps-speed\r\n");
     break;    
  } 
  switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_T2)) {
    case EEPROM_VALUE_MAP_DATA_T2_BATTERY_REMAINING:
     console_print("batt_remain routed to t2\r\n");
     break;
    case EEPROM_VALUE_MAP_DATA_T2_MISSION_CURRENT_SEQ:
     console_print("mission_seq routed to t2\r\n");
     break;
    case EEPROM_VALUE_MAP_DATA_T2_TEMPERATURE:
     console_print("temp routed to t2\r\n");
     break;
    case EEPROM_VALUE_MAP_DATA_T2_WP_DIST:
     console_print("wp_dist routed to t2\r\n");
     break;
   default:
     console_print("Invalid data routed to t2\r\n");
     break;    
  }    
}

void do_map(char* p) {
  if(strcmp(p, "vfas") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "direct") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_VFAS, EEPROM_VALUE_MAP_VFAS_DIRECT);
        console_print("voltage direct routed to vfas\r\n");
      } else if(strcmp(p, "average10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_VFAS, EEPROM_VALUE_MAP_VFAS_AVERAGE10);
        console_print("voltage average10 routed to vfas\r\n");
      } else if(strcmp(p, "average50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_VFAS, EEPROM_VALUE_MAP_VFAS_AVERAGE50);
        console_print("voltage average50 routed to vfas\r\n");
      }
    }
  } else if(strcmp(p, "current") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "direct") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT, EEPROM_VALUE_MAP_CURRENT_DIRECT);
        console_print("current direct routed to current\r\n");
      } else if(strcmp(p, "average10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT, EEPROM_VALUE_MAP_CURRENT_AVERAGE10);
        console_print("current average10 routed to current\r\n");
      } else if(strcmp(p, "average50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT, EEPROM_VALUE_MAP_CURRENT_AVERAGE50);
        console_print("current average50 routed to current\r\n");
      }
    }
  } else if(strcmp(p, "accx") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "direct") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_DIRECT);
        console_print("accx direct routed to accx\r\n");          
      } else if(strcmp(p, "average10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_AVERAGE10);
        console_print("accx average10 routed to accx\r\n");
      } else if(strcmp(p, "average50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_AVERAGE50);
        console_print("accx average50 routed to accx\r\n");
      } else if(strcmp(p, "peak10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10);
        console_print("accx peak10 routed to accx\r\n");
      } else if(strcmp(p, "peak50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE50);
        console_print("accx peak50 routed to accx\r\n");
      }
    }      
  } else if(strcmp(p, "accy") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "direct") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_DIRECT);
        console_print("accy direct routed to accy\r\n");
      } else if(strcmp(p, "average10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_AVERAGE10);
        console_print("accy average10 routed to accy\r\n");
      } else if(strcmp(p, "average50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_AVERAGE50);
        console_print("accy average50 routed to accy\r\n");
      } else if(strcmp(p, "peak10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10);
        console_print("accy peak10 routed to accy\r\n");
      } else if(strcmp(p, "peak50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE50);
        console_print("accy peak50 routed to accy\r\n");
      }
    }      
  } else if(strcmp(p, "accz") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "direct") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_DIRECT);
        console_print("accz direct routed to accz\r\n");
      } else if(strcmp(p, "average10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_AVERAGE10);
        console_print("accz average10 routed to accz\r\n");
      } else if(strcmp(p, "average50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_AVERAGE50);
        console_print("accz average50 routed to accz\r\n");
      } else if(strcmp(p, "peak10") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10);
        console_print("accz peak50 routed to accz\r\n");
      } else if(strcmp(p, "ptop50") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE50);
        console_print("accz peak50 routed to accz\r\n");
      }
    }
  } else if(strcmp(p, "gpsspeed") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "kph") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED, EEPROM_VALUE_MAP_GPS_SPEED_KPH);
        console_print("gpsspeed mps routed to gps-speed\r\n");
      } else if(strcmp(p, "mps") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED, EEPROM_VALUE_MAP_GPS_SPEED_KPH);
        console_print("gpsspeed kph routed to gps-speed\r\n");
      }
    }
  } else if(strcmp(p, "t2") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      if(strcmp(p, "batt_remain") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_BATTERY_REMAINING);
        console_print("batt_remain routed to t2\r\n");
      } else if(strcmp(p, "batt_remain") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_MISSION_CURRENT_SEQ);
        console_print("mission_seq routed to t2\r\n");
      } else if(strcmp(p, "batt_remain") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_TEMPERATURE);
        console_print("temp routed to t2\r\n");
      } else if(strcmp(p, "batt_remain") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_WP_DIST);
        console_print("wp_dist routed to t2\r\n");
      } else if(strcmp(p, "hdop") == 0) {
        EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_HDOP);
        console_print("hdop routed to t2\r\n");
      }
    }
  }
}

void do_set() {
  uint8_t n;
  char* p;
  
  p = strtok(NULL, " ");
  if(strcmp(p, "hdop") == 0) {
    p = strtok(NULL, " ");
    if(p != NULL) {
      n = atoi(p);
      EEPROM.write(EEPROM_ADDR_HDOP_THRESHOLD, n);
    }
    n = EEPROM.read(EEPROM_ADDR_HDOP_THRESHOLD);
    console_print("hdop threshold set to %d\r\n", n);
  }
}

void do_frsky() {
  uint8_t n;
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
  }
}

void do_factory() {
  telem_data_factory_reset();
  console_print("Settings restored to factory values\r\n");
}

void do_command(char *cmd_buffer) {
  int i = 0;
  uint8_t c;
  char* p;
  
  p = strtok(cmd_buffer, " ");
  if(p != NULL) {
    if(strcmp(p, "debug") == 0) {
      p = strtok(NULL, " ");
      if(strcmp(p, "mav") == 0) {
        p = strtok(NULL, " ");
        if(strcmp(p, "all") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavAllEnable, "Mav All");
        } else if (strcmp(p, "heartbeat") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavHeartbeatEnable, "Mav Heartbeat");
        } else if (strcmp(p, "gps") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavGpsEnable, "Mav GPS");
        } else if (strcmp(p, "attitude") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavAttitudeEnable, "Mav Attitude");
        } else if (strcmp(p, "imu") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavImuEnable, "Mav IMU");            
        } else if (strcmp(p, "vfr") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavVfrEnable, "Mav VFR");
        } else if (strcmp(p, "status") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavStatusEnable, "Mav Status");
        } else if (strcmp(p, "text") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavTextEnable, "Mav Text");
        } else if (strcmp(p, "other") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugMavOtherEnable, "Mav Other");
        } else {
          console_print("Unknown parameter %s\r\n", p);
        }
      } else if(strcmp(p, "frsky") == 0) {
        p = strtok(NULL, " ");
        if(strcmp(p, "all") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugFrskyAllEnable, "FrSky All");
        } else if (strcmp(p, "rpm") == 0) {
            p = strtok(NULL, " ");
            parse_debug_on_off(p, &debugFrskyRpmEnable, "FrSky RPM");
        } 
      } else if(strcmp(p, "temp") == 0) {
        p = strtok(NULL, " ");
        parse_debug_on_off(p, &debugTempEnable, "Temp");
      }
    } else if(strcmp(p, "dump") == 0) {
      do_dump();
    } else if(strcmp(p, "timing") == 0) {
      do_times();
    } else if(strcmp(p, "msg") == 0) {
      do_msg(cmd_buffer+4);
    } else if(strcmp(p, "map") == 0) {
       p = strtok(NULL, " ");
       if(p == NULL) {
         do_map_dump();
       } else {
         do_map(p);
       }
    } else if(strcmp(p, "set") == 0) {
       do_set();
    } else if(strcmp(p, "frsky") == 0) {
       do_frsky();
    } else if(strcmp(p, "factory") == 0) {
      do_factory();
    } else if(strcmp(p, "help") == 0) {
      do_help();
    } else {
      console_print("Unknown command\r\n");
    }
  }
}

void check_for_console_command() {
  while(DEBUG_SERIAL.available()) { 
    uint8_t c = DEBUG_SERIAL.read();

    if(c == '\r') {
      DEBUG_SERIAL.write("\r\n");
      cmd_buffer[cmd_index++] = '\0';
      cmd_index = 0;
      do_command(cmd_buffer);
      DEBUG_SERIAL.write("]");
    } else {
      DEBUG_SERIAL.write(c);
      cmd_buffer[cmd_index++] = tolower(c);
    }
  }
}
