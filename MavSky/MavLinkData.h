#ifndef MAVLINKDATA_H
#define MAVLINKDATA_H

#include <GCS_MAVLink.h>
#include <WProgram.h> 

#define MAVLINK_SERIAL  Serial2
#define MAV_HISTORY_BUFFER_SIZE 64

class MavLinkData {       
  private:  
    uint8_t    mavlink_buffer[MAVLINK_MAX_PACKET_LEN];
    
    int16_t    battery_current_buffer[MAV_HISTORY_BUFFER_SIZE];
    int16_t    battery_current_buffer_start = 0;
    int16_t    battery_current_buffer_length = 0;
    
    int16_t    battery_voltage_buffer[MAV_HISTORY_BUFFER_SIZE];
    int16_t    battery_voltage_buffer_start = 0;
    int16_t    battery_voltage_buffer_length = 0;

    uint32_t   last_process_100_millisecond_time = 0;
    uint32_t   last_process_1000_gps_latitude = 0;
    uint32_t   last_process_1000_gps_longitude = 0;
    
    double degrees_to_radians(double degrees);
    double radians_to_degrees(double radians);
    double get_distance_between_coordinates_int(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2);
    double get_distance_between_coordinates_double(double lat1, double lon1, double lat2, double lon2) ;
    double get_bearing_to_coordinates_int(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2);
    double get_bearing_to_coordinates_double(double lat1, double lon1, double lat2, double lon2);
           
  public:
    const double COORD_DEGREE_TO_INT_MULTIPLIER = 10000000.0;
    
    //  MAVLINK_MSG_ID_HEARTBEAT 
    uint8_t   heartbeat_type = 0;
    uint8_t   autopilot = 0;
    uint8_t   base_mode = 0;           
    uint16_t  custom_mode = 0;  
    uint8_t   system_status = 0;
    uint8_t   mavlink_version = 0;        
    
    // MAVLINK_MSG_ID_SYS_STATUS 
    uint16_t  battery_voltage = 0;        // 1000 = 1V
    int16_t   battery_current = 0;        // 10 = 1A
    int8_t    battery_remaining = 0;      // Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot estimate the remaining battery
    
    // MAVLINK_MSG_ID_GPS_RAW_INT 
    uint8_t   gps_fixtype = 0;            // 0= No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix
    uint8_t   gps_satellites_visible = 0; // number of visible satelites
    int32_t   gps_latitude = 0;           // 585522540;
    int32_t   gps_longitude = 0;          // 162344467;
    int32_t   gps_altitude = 0;           // 1000 = 1m
    int32_t   gps_speed = 0;              // in cm/s
    uint16_t  gps_hdop = 0;               // GPS HDOP horizontal dilution of position in cm
    uint16_t  mav_cog = 0;                // not heading but actual course in degrees.  If unknown UINT16_MAX
    
    // MAVLINK_MSG_ID_VFR_HUD 
    uint32_t  groundspeed = 0;            // Current ground speed in m/s
    uint32_t  heading = 0;                // Current heading in degrees, in compass units (0..360, 0=north)
    uint16_t  throttle = 0;               // Current throttle setting in integer percent, 0 to 100
    int32_t   bar_altitude = 0;          
    int32_t   climb_rate = 0;             
    
    // MAVLINK_MSG_ID_RAW_IMU
    int16_t   imu_xacc = 0;
    int16_t   imu_yacc = 0;
    int16_t   imu_zacc = 0;
  
    // MAVLINK_MSG_ID_ATTITUDE
    int32_t   roll_angle = 0;              // Roll angle (deg -180/180)
    int32_t   pitch_angle = 0;             // Pitch angle (deg -180/180)
    uint32_t  yaw_angle = 0;               // Yaw angle (rad)
  
    // MAVLINK_MSG_ID_MISSION_CURRENT
    uint16_t  mission_current_seq = 0;
    
    // MAVLINK_MSG_ID_SCALED_PRESSURE
    uint16_t  temperature = 0;
    
    // MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT
    uint16_t  wp_dist = 0;                 // meters
    
    // MAVLINK_MSG_ID_STATUSTEXT
    uint16_t  ap_status_severity;
    mavlink_statustext_t statustext;
    
    // MAVLINK_MSG_ID_RANGEFINDER
    int32_t   rangefinder_distance = 0;
  
    // MAVLINK_MSG_ID_BATTERY_STATUS
    uint16_t  current_consumed = 0;    
    uint16_t  energy_consumed = 0;    


    // MAVLINK_MSG_ID_RC_CHANNELS_RAW 
    uint16_t rc1 = 0;
    uint16_t rc2 = 0;
    uint16_t rc3 = 0;
    uint16_t rc4 = 0;
    uint16_t rc5 = 0;
    uint16_t rc6 = 0;
    uint16_t rc7 = 0;
    uint16_t rc8 = 0;
  
    // Calculated
    uint16_t  average_battery_voltage = 0;          
    int16_t   average_battery_current = 0;       
    int32_t   armed_latitude = 0;               
    int32_t   armed_longitude = 0;
    uint32_t  armed_distance = 0;           // in m
    uint16_t  armed_bearing = 0;            // in degrees (0-359)  
    uint16_t  calced_cog = 0;                 // in degrees (0-359)  
    
    uint32_t  tenth_amp_per_millisecond_consumed = 0;  
    
    MavLinkData();
    ~MavLinkData();
    void start_mavlink_packet_type(mavlink_message_t* msg_ptr, uint8_t stream_id, uint16_t rate);
    void request_mavlink_data(mavlink_message_t* msg_ptr);
    void mavlink_average_push(int16_t data, int16_t* p_buffer, int16_t* p_start, int16_t* p_length, int16_t max_length);
    int16_t mavlink_get_average(int16_t* p_buffer, int16_t start, int16_t length, int16_t use_samples, int16_t max_length);
    int mavlink_online();
    int mavlink_sys_status_data_valid();
    int mavlink_gps_data_valid();
    int mavlink_vfr_data_valid();
    int mavlink_imu_data_valid();
    int mavlink_attitude_data_valid();
    int mavlink_rangefinder_data_valid();
    void process_mavlink_packets();  
    void process_1000_millisecond(); 
    void process_100_millisecond(); 
    uint16_t calc_mah_consumed(); 
};


#endif


