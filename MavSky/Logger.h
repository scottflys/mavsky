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
#ifndef LOGGER_H
#define LOGGER_H

#include <WProgram.h> 

#define LOG_FRSKY_VARIO     30
#define LOG_FRSKY_FLVSS     31
#define LOG_FRSKY_FAS       32
#define LOG_FRSKY_GPS       33
#define LOG_FRSKY_RPM       34
#define LOG_FRSKY_SP2UH     35
#define LOG_FRSKY_SP2UR     36
#define LOG_TEMP            40

#define MAX_TIMESTAMPS      17
    
class Logger {     
  int32_t previous_time[MAX_TIMESTAMPS];
  int32_t current_time[MAX_TIMESTAMPS];
  
  public:  

    static const byte TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT         = 0;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT        = 1;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS        = 2;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT       = 3;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU           = 4;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE          = 5;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD           = 6;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT   = 7;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE   = 8;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT = 9;
    static const byte TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER       = 10; 
    static const byte TIMESTAMP_FRSKY_REQUEST                    = 11;
    static const byte TIMESTAMP_FRSKY_VARIO                      = 12;
    static const byte TIMESTAMP_FRSKY_FAS                        = 13;
    static const byte TIMESTAMP_FRSKY_GPS                        = 14;
    static const byte TIMESTAMP_FRSKY_RPM                        = 15;
    static const byte TIMESTAMP_FRSKY_OTHER                      = 16;

    static const byte LOG_MAV_HEARTBEAT   = 1;
    static const byte LOG_MAV_GPS         = 2;
    static const byte LOG_MAV_ATTITUDE    = 3;
    static const byte LOG_MAV_IMU         = 4;
    static const byte LOG_MAV_VFR         = 5;
    static const byte LOG_MAV_STATUS      = 6;
    static const byte LOG_MAV_TEXT        = 7;
    static const byte LOG_MAV_OTHER       = 8;
    static const byte LOG_MAV_UNKNOWN     = 9;
    static const byte LOG_MAV_RANGEFINDER = 10;

    
    int debugMavAllEnable = 0;
    int debugMavHeartbeatEnable = 0;
    int debugMavGpsEnable = 0;
    int debugMavImuEnable = 0;
    int debugMavVfrEnable = 0;
    int debugMavAttitudeEnable = 0;
    int debugMavStatusEnable = 0;
    int debugMavTextEnable = 0;
    int debugMavOtherEnable = 0;
    int debugMavRangeFinderEnable = 0;  
    int debugFrskyAllEnable = 0;
    int debugFrskyRpmEnable = 0;
    int debugFrskyVarioEnable = 0; 
    int debugFrskyFasEnable = 0;   
    int debugTempEnable = 0;
    
    Logger();
    ~Logger();
    void debug_print(int subsystem, char* fmt, ...);
    void add_timestamp(int stamp_id);
    int32_t get_timestamp_delta(int stamp_id);
    int32_t get_timestamp_age(int stamp_id);
};


#endif

