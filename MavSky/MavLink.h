
#ifndef MAVDATA
#define MAVDATA

#define MAV_HISTORY_BUFFER_SIZE 64
          
typedef struct MavClass {
  //  MAVLINK_MSG_ID_HEARTBEAT 
  uint8_t    base_mode;           
  uint16_t   custom_mode;         
  
  // Message # 1  MAVLINK_MSG_ID_SYS_STATUS 
  uint16_t   battery_voltage;            // 1000 = 1V
  int16_t    battery_current;            //  10 = 1A
  int8_t     battery_remaining;          // Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot estimate the remaining battery
  
  // MAVLINK_MSG_ID_GPS_RAW_INT 
  uint8_t    gps_fixtype;                // 0= No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix
  uint8_t    gps_satellites_visible;     // number of visible satelites
  int32_t    gps_latitude;               // 585522540;
  int32_t    gps_longitude;              // 162344467;
  int32_t    gps_altitude;               // 1000 = 1m
  int32_t    gps_speed;                  // in cm/s
  uint16_t   gps_hdop;                   // GPS HDOP horizontal dilution of position in cm
  
  // MAVLINK_MSG_ID_VFR_HUD 
  uint32_t   groundspeed;                // Current ground speed in m/s
  uint32_t   heading;                    // Current heading in degrees, in compass units (0..360, 0=north)
  uint16_t   throttle;                   // Current throttle setting in integer percent, 0 to 100
  int32_t    bar_altitude;               // 100 = 1m
  int32_t    ap_climb_rate;              // 100= 1m/s
  
  // MAVLINK_MSG_ID_RAW_IMU
  int16_t    imu_xacc;
  int16_t    imu_yacc;
  int16_t    imu_zacc;

  // MAVLINK_MSG_ID_ATTITUDE
  int32_t    roll_angle;                 // Roll angle (deg -180/180)
  int32_t    pitch_angle;                // Pitch angle (deg -180/180)
  uint32_t   yaw_angle;                  // Yaw angle (rad)

  // MAVLINK_MSG_ID_MISSION_CURRENT
  uint16_t   mission_current_seq = 0;
  
  // MAVLINK_MSG_ID_SCALED_PRESSURE
  uint16_t   temperature = 0;
  
  // MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT
  uint16_t   wp_dist = 0;                // meters
  
  // MAVLINK_MSG_ID_STATUSTEXT
  uint16_t   ap_status_severity;
  mavlink_statustext_t statustext;
  
  // Calculated
  
  int16_t    battery_current_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    battery_current_buffer_start = 0;
  int16_t    battery_current_buffer_length = 0;
  
  int16_t    battery_voltage_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    battery_voltage_buffer_start = 0;
  int16_t    battery_voltage_buffer_length = 0;
  
  int16_t    imu_xacc_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_xacc_buffer_start = 0;
  int16_t    imu_xacc_buffer_length = 0;
  
  int16_t    imu_yacc_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_yacc_buffer_start = 0;
  int16_t    imu_yacc_buffer_length = 0;
  
  int16_t    imu_zacc_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_zacc_buffer_start = 0;
  int16_t    imu_zacc_buffer_length = 0;
    
  int16_t    imu_xacc_peak_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_xacc_peak_buffer_start = 0;
  int16_t    imu_xacc_peak_buffer_length = 0;
  
  int16_t    imu_yacc_peak_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_yacc_peak_buffer_start = 0;
  int16_t    imu_yacc_peak_buffer_length = 0;
  
  int16_t    imu_zacc_peak_buffer[MAV_HISTORY_BUFFER_SIZE];
  int16_t    imu_zacc_peak_buffer_start = 0;
  int16_t    imu_zacc_peak_buffer_length = 0;
  
  int16_t    imu_xacc_previous = 0;
  int16_t    imu_xacc_lowest = 0;
  int16_t    imu_xacc_highest = 0;
  int16_t    imu_xacc_peak;  
  
  int16_t    imu_yacc_previous = 0;
  int16_t    imu_yacc_lowest = 0;
  int16_t    imu_yacc_highest = 0;
  int16_t    imu_yacc_peak;  
  
  int16_t    imu_zacc_previous = 0;
  int16_t    imu_zacc_lowest = 0;
  int16_t    imu_zacc_highest = 0;
  int16_t    imu_zacc_peak;
  
} MavClass;

MavClass mav;
#endif

extern MavClass mav;



