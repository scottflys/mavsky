
#include "MavConsole.h"
#include "Logger.h"

extern Logger *logger;
extern MavConsole *console;

Logger::Logger() {
  int i;
  for(i=0; i<MAX_TIMESTAMPS; i++) {
    previous_time[i] = -9999999L;      // made very negative but leave some room in the long for comparing to current time
    current_time[i] = -9999999L;
  }
}

Logger::~Logger() {  
}
    
void Logger::debug_print(int subsystem, char* fmt, ...) {
  int print_enable = 0;
  if(subsystem == LOG_MAV_HEARTBEAT) {
    if(debugMavHeartbeatEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_MAV_GPS) {   
    if(debugMavGpsEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_MAV_IMU) {   
    if(debugMavImuEnable || debugMavAllEnable) {
      print_enable = 1;
    }  
  } else if(subsystem == LOG_MAV_ATTITUDE) {   
    if(debugMavAttitudeEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_MAV_STATUS) {   
    if(debugMavStatusEnable || debugMavAllEnable) {
      print_enable = 1;
    } 
  } else if(subsystem == LOG_MAV_TEXT) {   
    if(debugMavTextEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_MAV_OTHER) {   
    if(debugMavOtherEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_MAV_RANGEFINDER) {   
    if(debugMavRangeFinderEnable || debugMavAllEnable) {
      print_enable = 1;
    }
  } else if(subsystem == LOG_FRSKY_RPM) {   
    if(debugFrskyRpmEnable || debugFrskyAllEnable) {
      print_enable = 1;
    } 
  } else if(subsystem == LOG_FRSKY_VARIO) {   
    if(debugFrskyVarioEnable || debugFrskyAllEnable) {
      print_enable = 1;
    } 
  } else if(subsystem == LOG_FRSKY_FAS) {   
    if(debugFrskyFasEnable || debugFrskyAllEnable) {
      print_enable = 1;
    }      
  } else if(subsystem == LOG_TEMP) {   
    if(debugTempEnable) {
      print_enable = 1;
    }     
  }
  if(print_enable) {
    char formatted_string[256];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(formatted_string, fmt, argptr);
    va_end(argptr);
    if(console != NULL) {
      console->console_print("%d \t%s\r\n", millis(), formatted_string);
    }
  }
}

void Logger::add_timestamp(int stamp_id) {
  previous_time[stamp_id] = current_time[stamp_id];
  current_time[stamp_id] = millis();
}

int32_t Logger::get_timestamp_delta(int stamp_id) {
  return current_time[stamp_id] - previous_time[stamp_id];
}

int32_t Logger::get_timestamp_age(int stamp_id) {
  uint32_t age = millis() - current_time[stamp_id];            
  return age;
}
