#ifndef LOGGER
#define LOGGER

#define LOG_MAV_HEARTBEAT 1
#define LOG_MAV_GPS       2
#define LOG_MAV_ATTITUDE  3
#define LOG_MAV_IMU       4
#define LOG_MAV_VFR       5
#define LOG_MAV_STATUS    6
#define LOG_MAV_TEXT      7
#define LOG_MAV_OTHER     8
#define LOG_MAV_UNKNOWN   9
#define LOG_MAV_RANGEFINDER   10    //2015-08-22

#define LOG_FRSKY_VARIO     30
#define LOG_FRSKY_FLVSS     31
#define LOG_FRSKY_FAS       32
#define LOG_FRSKY_GPS       33
#define LOG_FRSKY_RPM       34
#define LOG_FRSKY_SP2UH     35
#define LOG_FRSKY_SP2UR     36
#define LOG_TEMP            40

#define MAX_TIMESTAMPS 17
#define TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT         0
#define TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT        1
#define TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS        2
#define TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT       3
#define TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU           4
#define TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE          5
#define TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD           6
#define TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT   7
#define TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE   8
#define TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT 9
#define TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER       10   

#define TIMESTAMP_FRSKY_REQUEST                    11
#define TIMESTAMP_FRSKY_VARIO                      12
#define TIMESTAMP_FRSKY_FAS                        13
#define TIMESTAMP_FRSKY_GPS                        14
#define TIMESTAMP_FRSKY_RPM                        15
#define TIMESTAMP_FRSKY_OTHER                      16

extern int debugMavAllEnable;
extern int debugMavHeartbeatEnable;
extern int debugMavGpsEnable;
extern int debugMavImuEnable;
extern int debugMavAttitudeEnable;
extern int debugMavVfrEnable;
extern int debugMavStatusEnable;
extern int debugMavTextEnable;
extern int debugMavOtherEnable;
extern int debugMavRangeFinderEnable; 
extern int debugFrskyAllEnable;
extern int debugFrskyVarioEnable;
extern int debugFrskyFasEnable;     
extern int debugFrskyRpmEnable;
extern int debugTempEnable;

void debug_print(int subsystem, char* fmt, ...);

#endif

