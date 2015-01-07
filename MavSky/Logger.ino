

int32_t previous_time[MAX_TIMESTAMPS];
int32_t current_time[MAX_TIMESTAMPS];

int debugMavAllEnable = 0;
int debugMavHeartbeatEnable = 0;
int debugMavGpsEnable = 0;
int debugMavImuEnable = 0;
int debugMavVfrEnable = 0;
int debugMavAttitudeEnable = 0;
int debugMavStatusEnable = 0;
int debugMavTextEnable = 0;
int debugMavOtherEnable = 0;
int debugFrskyAllEnable = 0;
int debugFrskyRpmEnable = 0;
int debugTempEnable = 0;

void debug_init() {
  DEBUG_SERIAL.begin(57600);
  int i;
  for(i=0; i<MAX_TIMESTAMPS; i++) {
    previous_time[i] = -9999999L;      // made very negative but leave some room in the long for comparing to current time
    current_time[i] = -9999999L;
  }
}

void debug_print(int subsystem, char* fmt, ...) {
  int print_enable = 0;
  switch(subsystem) {
    case LOG_MAV_HEARTBEAT :
      if(debugMavHeartbeatEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;    
    case LOG_MAV_GPS :
      if(debugMavGpsEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;    
    case LOG_MAV_IMU :
      if(debugMavImuEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;    
    case LOG_MAV_ATTITUDE :
      if(debugMavAttitudeEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;      
    case LOG_MAV_STATUS :
      if(debugMavStatusEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;    
    case LOG_MAV_TEXT :
      if(debugMavTextEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break;
    case LOG_MAV_OTHER :
      if(debugMavOtherEnable || debugMavAllEnable) {
        print_enable = 1;
      }
      break; 
     case LOG_FRSKY_RPM :
      if(debugFrskyRpmEnable || debugFrskyAllEnable) {
        print_enable = 1;
      } 
      break;         
    case LOG_TEMP :
      if(debugTempEnable) {
        print_enable = 1;
      }
      break;      
  }
  if(print_enable) {
    char formatted_string[256];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(formatted_string, fmt, argptr);
    va_end(argptr);
    console_print("%d \t%s\r\n", millis(), formatted_string);
  }
}

void add_timestamp(int stamp_id) {
  previous_time[stamp_id] = current_time[stamp_id];
  current_time[stamp_id] = millis();
}

int32_t get_timestamp_delta(int stamp_id) {
  return current_time[stamp_id] - previous_time[stamp_id];
}

int32_t get_timestamp_age(int stamp_id) {
  uint32_t age = millis() - current_time[stamp_id];            
  return age;
}
