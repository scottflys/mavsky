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
#include <GCS_MAVLink.h>
//#include "MavLink.h"
#include "Logger.h"
#include "MavConsole.h"
#include "DataBroker.h"

extern void frsky_send_text_message(char *msg);
extern Logger *logger;
extern MavConsole *console;

#define EXPIRY_MILLIS_MAVLINK_MSG_ID_HEARTBEAT       3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_SYS_STATUS      3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_GPS_RAW_INT     3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_VFR_HUD         3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_RAW_IMU         3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_ATTITUDE        3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_RANGEFINDER     3000
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_RC_CHANNELS_RAW 3000

#define STATUS_TEXT_MAX             128
#define START_MAVLINK_PACKETS       1

char status_text_buffer[STATUS_TEXT_MAX];

MavLinkData::MavLinkData() {
  MAVLINK_SERIAL.begin(57600);
}

void MavLinkData::mavlink_average_push(int16_t data, int16_t* p_buffer, int16_t* p_start, int16_t* p_length, int16_t max_length) {
  p_buffer[(*p_start + *p_length) % max_length] = data;
  if(*p_length >= max_length) {
    *p_start = (*p_start + 1) % max_length;
  } else {
    (*p_length)++;
  }
}

int16_t MavLinkData::mavlink_get_average(int16_t* p_buffer, int16_t start, int16_t length, int16_t use_samples, int16_t max_length) {
  int16_t i;
  int32_t sum = 0;
  
  if(length < use_samples) {
    use_samples = length;
  }
  uint16_t start_index = length - use_samples;
  for(i=start_index; i<length; i++) {
      sum += p_buffer[(start + i) % max_length];
  }
  int16_t round_val;
  if(sum >= 0) {
    round_val = use_samples / 2;
  } else {
    round_val = (0 - use_samples) / 2;
  }
  return (int16_t)((sum + round_val)/ use_samples);                     // round
}

int MavLinkData::mavlink_heartbeat_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT) < EXPIRY_MILLIS_MAVLINK_MSG_ID_HEARTBEAT) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_sys_status_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS) < EXPIRY_MILLIS_MAVLINK_MSG_ID_SYS_STATUS) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_gps_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT) < EXPIRY_MILLIS_MAVLINK_MSG_ID_GPS_RAW_INT) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_vfr_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD) < EXPIRY_MILLIS_MAVLINK_MSG_ID_VFR_HUD) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_imu_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU) < EXPIRY_MILLIS_MAVLINK_MSG_ID_RAW_IMU) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_attitude_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE) < EXPIRY_MILLIS_MAVLINK_MSG_ID_ATTITUDE) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_rangefinder_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER) < EXPIRY_MILLIS_MAVLINK_MSG_ID_RANGEFINDER) {
    return 1;
  } else {
    return 0;
  }
}

int MavLinkData::mavlink_rc_channels_raw_data_valid() {
  if(logger->get_timestamp_age(Logger::TIMESTAMP_MAVLINK_MSG_ID_RC_CHANNELS_RAW) < EXPIRY_MILLIS_MAVLINK_MSG_ID_RC_CHANNELS_RAW) {
    return 1;
  } else {
    return 0;
  }
}

uint16_t MavLinkData::calc_mah_consumed() {
  return tenth_amp_per_millisecond_consumed / 36000L;
}

void MavLinkData::process_1000_millisecond() {
  uint8_t armed_bit;
  

  armed_bit = (base_mode >> 7) & 1;
  if(armed_bit) {
    if(gps_speed > 50) {
      calced_cog = round(get_bearing_to_coordinates_int(last_process_1000_gps_latitude, last_process_1000_gps_longitude, gps_latitude, gps_longitude)); 
    }
    calced_distance_travelled += gps_speed;  
  }         
  last_process_1000_gps_latitude = gps_latitude;
  last_process_1000_gps_longitude = gps_longitude;
}

void MavLinkData::process_100_millisecond() {
  uint32_t current_milli = millis();
  uint32_t delta = current_milli - last_process_100_millisecond_time;
  if(last_process_100_millisecond_time != 0) {
    tenth_amp_per_millisecond_consumed += (average_battery_current * delta); 
  }
  last_process_100_millisecond_time = current_milli;
}
    
double MavLinkData::degrees_to_radians(double degrees) {
  return degrees * (double)3.14159265358979323846 / (double)180.0;
}
    
double MavLinkData::radians_to_degrees(double radians) {
  return radians * (double)180.0 / (double)3.14159265358979323846;
}

// http://www.movable-type.co.uk/scripts/latlong.html
double MavLinkData::get_distance_between_coordinates_double(double lat1, double lon1, double lat2, double lon2) {
  double theta1 = degrees_to_radians(lat1);              
  double theta2 = degrees_to_radians(lat2);
  double delta_theta = degrees_to_radians(lat2 - lat1);
  double delta_lambda = degrees_to_radians(lon2 - lon1);
  double a = sin(delta_theta / 2.0) * sin(delta_theta / 2.0) + cos(theta1) * cos(theta2) * sin(delta_lambda / 2.0) * sin(delta_lambda / 2.0);
  double c = 2.0 * atan2(sqrt(a), sqrt((double)1.0 - a));
  double d = 6371000.0 * c;
  return d;
}

double MavLinkData::get_distance_between_coordinates_int(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2) {
  if (lat1 == 0 || lon1 == 0 || lat2 == 0 || lon2 == 0) {
      return 0;
  }
  return get_distance_between_coordinates_double((double)lat1 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lon1 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lat2 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lon2 / COORD_DEGREE_TO_INT_MULTIPLIER);
}

double MavLinkData::get_bearing_to_coordinates_double(double lat1, double lon1, double lat2, double lon2) {
  double theta1 = degrees_to_radians(lat1);              
  double theta2 = degrees_to_radians(lat2);
  double lambda1 = degrees_to_radians(lon1);              
  double lambda2 = degrees_to_radians(lon2);    
  double y = sin(lambda2 - lambda1) * cos(theta2);
  double x = cos(theta1) * sin(theta2) - sin(theta1) * cos(theta2) * cos(lambda2 - lambda1);
  double bearing = radians_to_degrees(atan2(y, x));
  if (bearing < 0.0) {
    bearing += 360.0;
  }
  return bearing;
}

double MavLinkData::get_bearing_to_coordinates_int(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2) {
  if (lat1 == 0 || lon1 == 0 || lat2 == 0 || lon2 == 0) {
      return 0;
  }
  return get_bearing_to_coordinates_double((double)lat1 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lon1 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lat2 / COORD_DEGREE_TO_INT_MULTIPLIER, (double)lon2 / COORD_DEGREE_TO_INT_MULTIPLIER);
}

void MavLinkData::start_mavlink_packet_type(mavlink_message_t* msg_ptr, uint8_t stream_id, uint16_t rate) {
  uint16_t byte_length;

  mavlink_msg_request_data_stream_pack(0xFF, 0xBE, msg_ptr, 1, 1, stream_id, rate, START_MAVLINK_PACKETS);
  byte_length = mavlink_msg_to_send_buffer(mavlink_buffer, msg_ptr);
  MAVLINK_SERIAL.write(mavlink_buffer, byte_length);
  delay(10);
}

void MavLinkData::start_mavlink_if_stopped(mavlink_message_t* msg_ptr) {
  static uint32_t initializing_timeout = 0;

  if(millis() > initializing_timeout) {
    if(!mavlink_heartbeat_data_valid()) {
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_RAW_SENSORS, 2);   
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_EXTENDED_STATUS, 3);
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_RAW_CONTROLLER, 0);
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_POSITION, 3);  
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_EXTRA1, 5);  
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_EXTRA2, 2); 
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_EXTRA3, 3);
      initializing_timeout = millis() + 1000L;                          
    } else if(!mavlink_rc_channels_raw_data_valid()) {     
      start_mavlink_packet_type(msg_ptr, MAV_DATA_STREAM_RC_CHANNELS, 3); 
      initializing_timeout = millis() + 1000L;                    
    }
  }
}

void MavLinkData::process_mavlink_packets() { 
  uint8_t armed_bit = 0;
  mavlink_message_t msg;
  mavlink_status_t status;

  start_mavlink_if_stopped(&msg);

  while(MAVLINK_SERIAL.available()) { 
    uint8_t c = MAVLINK_SERIAL.read();
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      switch(msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT);
          logger->debug_print(Logger::LOG_MAV_HEARTBEAT, (char *)"MAVLINK_MSG_ID_HEARTBEAT: base_mode: %d, custom_mode: %d", mavlink_msg_heartbeat_get_base_mode(&msg), mavlink_msg_heartbeat_get_custom_mode(&msg));     
          heartbeat_type = mavlink_msg_heartbeat_get_type(&msg);
          autopilot = mavlink_msg_heartbeat_get_autopilot(&msg);
          base_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
          custom_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
          system_status = mavlink_msg_heartbeat_get_system_status(&msg);
          mavlink_version = mavlink_msg_heartbeat_get_mavlink_version(&msg);      
          break;
  
        case MAVLINK_MSG_ID_STATUSTEXT:       
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT);
          logger->debug_print(Logger::LOG_MAV_TEXT, (char *)"MAVLINK_MSG_ID_STATUSTEXT: severity: %ld text: %s", statustext.severity, statustext.text);
          mavlink_msg_statustext_decode(&msg, &statustext);
          sprintf(status_text_buffer, "%d%s", statustext.severity & 0x7, statustext.text);
          DataBroker::extended_message_processor.frsky_send_text_message(status_text_buffer);                  
          break;
  
        case MAVLINK_MSG_ID_SYS_STATUS :        
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS);
          logger->debug_print(Logger::LOG_MAV_STATUS, (char *)"MAVLINK_MSG_ID_SYS_STATUS: voltage_battery: %d, current_battery: %d", mavlink_msg_sys_status_get_voltage_battery(&msg), mavlink_msg_sys_status_get_current_battery(&msg));
          battery_voltage = max(mavlink_msg_sys_status_get_voltage_battery(&msg), 0);             
          battery_current = max(mavlink_msg_sys_status_get_current_battery(&msg), 0);    
          mavlink_average_push(battery_voltage, battery_voltage_buffer, &battery_voltage_buffer_start, &battery_voltage_buffer_length, MAV_HISTORY_BUFFER_SIZE);    
          mavlink_average_push(battery_current, battery_current_buffer, &battery_current_buffer_start, &battery_current_buffer_length, MAV_HISTORY_BUFFER_SIZE);
          average_battery_voltage = mavlink_get_average(battery_voltage_buffer, battery_voltage_buffer_start, battery_voltage_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE) / 10;
          average_battery_current = mavlink_get_average(battery_current_buffer, battery_current_buffer_start, battery_current_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE) / 10;         
          battery_remaining = mavlink_msg_sys_status_get_battery_remaining(&msg); //battery capacity reported in %
          //debug_print(LOG_MAV_STATUS, "MAVLINK_MSG_ID_SYS_STATUS: #1");
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_present: %d", mavlink_msg_sys_status_get_onboard_control_sensors_present(&msg));
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_enabled: %d", mavlink_msg_sys_status_get_onboard_control_sensors_enabled(&msg));
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_health: %d", mavlink_msg_sys_status_get_onboard_control_sensors_health(&msg));
          //debug_print(LOG_MAV_STATUS, "load: %d", mavlink_msg_sys_status_get_load(&msg));
          //debug_print(LOG_MAV_STATUS, "drop_rate_comm: %d", mavlink_msg_sys_status_get_drop_rate_comm(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_comm: %d", mavlink_msg_sys_status_get_errors_comm(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count1: %d", mavlink_msg_sys_status_get_errors_count1(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count2: %d", mavlink_msg_sys_status_get_errors_count2(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count3: %d", mavlink_msg_sys_status_get_errors_count3(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count4: %d", mavlink_msg_sys_status_get_errors_count4(&msg));
          break;
  
        case MAVLINK_MSG_ID_GPS_RAW_INT:     
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT);
          gps_fixtype = mavlink_msg_gps_raw_int_get_fix_type(&msg);                              // 0 = No GPS, 1 =No Fix, 2 = 2D Fix, 3 = 3D Fix
          gps_satellites_visible =  mavlink_msg_gps_raw_int_get_satellites_visible(&msg);      
          gps_hdop = mavlink_msg_gps_raw_int_get_eph(&msg);                                    // hdop * 100
          gps_latitude = mavlink_msg_gps_raw_int_get_lat(&msg);
          gps_longitude = mavlink_msg_gps_raw_int_get_lon(&msg);
          gps_altitude = mavlink_msg_gps_raw_int_get_alt(&msg);                                // 1m =1000
          gps_speed = mavlink_msg_gps_raw_int_get_vel(&msg);                                   // 100 = 1m/s
          mav_cog = mavlink_msg_gps_raw_int_get_cog(&msg);
          logger->debug_print(Logger::LOG_MAV_GPS, (char *)"MAVLINK_MSG_ID_GPS_RAW_INT: fixtype: %d, sats: %d, hdop: %d, speed: %ld, alt: %ld lat: %ld lon: %ld", gps_fixtype, gps_satellites_visible, gps_hdop, gps_speed, gps_altitude, gps_latitude, gps_longitude);
          armed_bit = (base_mode >> 7) & 1;
          if(armed_bit) {
            if(armed_latitude == 0 || armed_longitude == 0) {                                   // set first gps after arm
              armed_latitude = gps_latitude;
              armed_longitude = gps_longitude;                
            }
          } else {
            if(armed_latitude != 0 || armed_longitude != 0) {                                   // clear when disarmed
                armed_latitude = 0;
                armed_longitude = 0; 
                armed_distance = 0;
                armed_bearing = 0; 
            }
          }
          armed_distance = round(get_distance_between_coordinates_int(armed_latitude, armed_longitude, gps_latitude, gps_longitude));
          armed_bearing = round(get_bearing_to_coordinates_int(armed_latitude, armed_longitude, gps_latitude, gps_longitude));
          //debug_print(LOG_MAV_GPS, "time_usec: %d", mavlink_msg_gps_raw_int_get_time_usec(&msg));
          //debug_print(LOG_MAV_GPS, "eph: %d", mavlink_msg_gps_raw_int_get_eph(&msg));
          //debug_print(LOG_MAV_GPS, "epv: %d", mavlink_msg_gps_raw_int_get_epv(&msg));
          break;
  
        case MAVLINK_MSG_ID_RAW_IMU:
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU);
          logger->debug_print(Logger::LOG_MAV_IMU, (char *)"MAVLINK_MSG_ID_RAW_IMU: xacc: %d, yacc: %d, zacc: %d", mavlink_msg_raw_imu_get_xacc(&msg), mavlink_msg_raw_imu_get_yacc(&msg), mavlink_msg_raw_imu_get_zacc(&msg));
          //debug_print(LOG_MAV_IMU, "MAVLINK_MSG_ID_RAW_IMU: #27");
          //debug_print(LOG_MAV_IMU, "time_usec: %d", mavlink_msg_raw_imu_get_time_usec(&msg));
          //debug_print(LOG_MAV_IMU, "xacc: %d", mav.imu_xacc);
          //debug_print(LOG_MAV_IMU, "yacc: %d", mav.imu_yacc);
          //debug_print(LOG_MAV_IMU, "zacc: %d", mav.imu_zacc);
          //debug_print(LOG_MAV_IMU, "xgyro: %d", mavlink_msg_raw_imu_get_xgyro(&msg));
          //debug_print(LOG_MAV_IMU, "ygyro: %d", mavlink_msg_raw_imu_get_ygyro(&msg));
          //debug_print(LOG_MAV_IMU, "zgyro: %d", mavlink_msg_raw_imu_get_zgyro(&msg));
          //debug_print(LOG_MAV_IMU, "xmag: %d", mavlink_msg_raw_imu_get_xmag(&msg));
          //debug_print(LOG_MAV_IMU, "ymag: %d", mavlink_msg_raw_imu_get_ymag(&msg));
          //debug_print(LOG_MAV_IMU, "zmag: %d", mavlink_msg_raw_imu_get_zmag(&msg));
          break;
  
        case MAVLINK_MSG_ID_ATTITUDE:   
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE);
          logger->debug_print(Logger::LOG_MAV_ATTITUDE, (char *)"MAVLINK_MSG_ID_ATTITUDE: roll_angle: %f (%d deg), pitch_angle: %f (%d deg), yaw_angle: %f", mavlink_msg_attitude_get_roll(&msg), roll_angle, mavlink_msg_attitude_get_pitch(&msg), pitch_angle, mavlink_msg_attitude_get_yaw(&msg));
          roll_angle = mavlink_msg_attitude_get_roll(&msg) * 180 / 3.1416;      // rad to degrees
          if(abs(roll_angle) <= 90) {
           pitch_angle = mavlink_msg_attitude_get_pitch(&msg) * 180 / 3.1416; 
          } 
          else {
           pitch_angle = 180 - mavlink_msg_attitude_get_pitch(&msg) * 180 / 3.1416;   
          }
          yaw_angle = (mavlink_msg_attitude_get_yaw(&msg) + 3.1416) * 162.9747;
          //debug_print(LOG_MAV_ATTITUDE, "MAVLINK_MSG_ID_ATTITUDE: #30");
          //debug_print(LOG_MAV_ATTITUDE, "time_boot_ms: %d", mavlink_msg_attitude_get_time_boot_ms(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "roll: %d", mavlink_msg_attitude_get_roll(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "pitch: %d", mavlink_msg_attitude_get_pitch(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "yaw: %d", mavlink_msg_attitude_get_yaw(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "rollspeed: %d", mavlink_msg_attitude_get_rollspeed(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "pitchspeed: %d", mavlink_msg_attitude_get_pitchspeed(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "yawspeed: %d", mavlink_msg_attitude_get_yawspeed(&msg));
          break;
  
        case MAVLINK_MSG_ID_VFR_HUD:                              
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD);
          logger->debug_print(Logger::LOG_MAV_VFR, (char *)"MAVLINK_MSG_ID_VFR_HUD: groundspeed: %f, heading: %d, throttle: %d, alt: %f, climbrate: %f", mavlink_msg_vfr_hud_get_groundspeed(&msg), mavlink_msg_vfr_hud_get_heading(&msg), mavlink_msg_vfr_hud_get_throttle(&msg), mavlink_msg_vfr_hud_get_alt(&msg), mavlink_msg_vfr_hud_get_climb(&msg));
          groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);         
          heading = mavlink_msg_vfr_hud_get_heading(&msg);                 
          throttle = mavlink_msg_vfr_hud_get_throttle(&msg);           
          bar_altitude = mavlink_msg_vfr_hud_get_alt(&msg); 
          climb_rate = mavlink_msg_vfr_hud_get_climb(&msg);  
          break; 
  
        case MAVLINK_MSG_ID_MISSION_CURRENT:            
          mission_current_seq = mavlink_msg_mission_current_get_seq(&msg);
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK MSG_ID_MISSION_CURRENT");
          //debug_print(LOG_MAV_OTHER, "seq: %d", mav.mission_current_seq);     
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT);
          break;

        case MAVLINK_MSG_ID_SCALED_PRESSURE:             
          temperature = mavlink_msg_scaled_pressure_get_temperature(&msg);
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK MSG_ID_SCALED_PRESSURE");
          //debug_print(LOG_MAV_OTHER, "time_boot_ms: %d", mavlink_msg_scaled_pressure_get_time_boot_ms(&msg));
          //debug_print(LOG_MAV_OTHER, "press_abs: %d", mavlink_msg_scaled_pressure_get_press_abs(&msg));
          //debug_print(LOG_MAV_OTHER, "press_diff: %d", mavlink_msg_scaled_pressure_get_press_diff(&msg));
          //debug_print(LOG_MAV_OTHER, "temperature: %d",  mav.temperature);          
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE);
          break;

        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:          
          wp_dist = mavlink_msg_nav_controller_output_get_wp_dist(&msg);
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK MSG_ID_NAV_CONTROLLER_OUTPUT");
          //debug_print(LOG_MAV_OTHER, "nav_roll: %d", mavlink_msg_nav_controller_output_get_nav_roll(&msg));
          //debug_print(LOG_MAV_OTHER, "nav_pitch: %d", mavlink_msg_nav_controller_output_get_nav_pitch(&msg));
          //debug_print(LOG_MAV_OTHER, "nav_bearing: %d", mavlink_msg_nav_controller_output_get_nav_bearing(&msg));
          //debug_print(LOG_MAV_OTHER, "target_bearing: %d", mavlink_msg_nav_controller_output_get_target_bearing(&msg));
          //debug_print(LOG_MAV_OTHER, "wp_dist: %d", mav.wp_dist);
          //debug_print(LOG_MAV_OTHER, "alt_error: %d", mavlink_msg_nav_controller_output_get_alt_error(&msg));
          //debug_print(LOG_MAV_OTHER, "aspd_error: %d", mavlink_msg_nav_controller_output_get_aspd_error(&msg));
          //debug_print(LOG_MAV_OTHER, "xtrack_error: %d", mavlink_msg_nav_controller_output_get_xtrack_error(&msg));  
          logger->add_timestamp(Logger::TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT);
          break;
          
        case MAVLINK_MSG_ID_SYSTEM_TIME:       
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_SYSTEM_TIME");
          //debug_print(LOG_MAV_OTHER, "time_unix_usec: %d", mavlink_msg_system_time_get_time_unix_usec(&msg));
          //debug_print(LOG_MAV_OTHER, "time_boot_ms: %d", mavlink_msg_system_time_get_time_boot_ms(&msg));
          break;
          
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:      
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_GLOBAL_POSITION_INT");
          //debug_print(LOG_MAV_OTHER, "time_boot_ms: %d", mavlink_msg_global_position_int_get_time_boot_ms(&msg));
          //debug_print(LOG_MAV_OTHER, "lat: %d", mavlink_msg_global_position_int_get_lat(&msg));
          //debug_print(LOG_MAV_OTHER, "lon: %d", mavlink_msg_global_position_int_get_lon(&msg));
          //debug_print(LOG_MAV_OTHER, "alt: %d", mavlink_msg_global_position_int_get_alt(&msg));
          //debug_print(LOG_MAV_OTHER, "relative_alt: %d", mavlink_msg_global_position_int_get_relative_alt(&msg));
          //debug_print(LOG_MAV_OTHER, "vx: %d", mavlink_msg_global_position_int_get_vx(&msg));
          //debug_print(LOG_MAV_OTHER, "vy: %d", mavlink_msg_global_position_int_get_vy(&msg));
          //debug_print(LOG_MAV_OTHER, "vz: %d", mavlink_msg_global_position_int_get_vz(&msg));
          //debug_print(LOG_MAV_OTHER, "hdg: %d", mavlink_msg_global_position_int_get_hdg(&msg));
          break;

        case MAVLINK_MSG_ID_BATTERY_STATUS:          
          current_consumed = mavlink_msg_battery_status_get_current_consumed(&msg);    
          energy_consumed = mavlink_msg_battery_status_get_energy_consumed(&msg);    
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_BATTERY_STATUS");
          //debug_print(LOG_MAV_OTHER, "id: %d", mavlink_msg_battery_status_get_id(&msg));
          //debug_print(LOG_MAV_OTHER, "battery_function: %d", mavlink_msg_battery_status_get_battery_function(&msg));
          //debug_print(LOG_MAV_OTHER, "type: %d", mavlink_msg_battery_status_get_type(&msg));
          //debug_print(LOG_MAV_OTHER, "temperature: %d", mavlink_msg_battery_status_get_temperature(&msg));
          //debug_print(LOG_MAV_OTHER, "voltages: %d", mavlink_msg_battery_status_get_voltages(&msg));
          //debug_print(LOG_MAV_OTHER, "current_battery: %d", mavlink_msg_battery_status_get_current_battery(&msg));
          //debug_print(LOG_MAV_OTHER, "current_consumed: %d", mavlink_msg_battery_status_get_current_consumed(&msg));
          //debug_print(LOG_MAV_OTHER, "energy_consumed: %d", mavlink_msg_battery_status_get_energy_consumed(&msg));
          //debug_print(LOG_MAV_OTHER, "battery_remaining: %d", mavlink_msg_battery_status_get_battery_remaining(&msg));

          break;
        
        case MAVLINK_MSG_ID_SENSOR_OFFSETS:           
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_SENSOR_OFFSETS");
          break;
          
        case MAVLINK_MSG_ID_MEMINFO: 
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_MEMINFO");
          //debug_print(LOG_MAV_OTHER, "brkval: %d", mavlink_msg_meminfo_get_brkval(&msg));
          //debug_print(LOG_MAV_OTHER, "freemem: %d", mavlink_msg_meminfo_get_freemem(&msg));
          break;

        case MAVLINK_MSG_ID_AHRS:            
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_AHRS");
          break;

        case MAVLINK_MSG_ID_HWSTATUS:     
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_HWSTATUS");          
          break;

        case MAVLINK_MSG_ID_RANGEFINDER:                    
          logger->debug_print(logger->LOG_MAV_RANGEFINDER, (char *)"MAVLINK_MSG_ID_RANGEFINDER");
          rangefinder_distance = mavlink_msg_rangefinder_get_distance(&msg) * 100;   //RANGEFINDER distance in cm
          //debug_print(LOG_MAV_RANGEFINDER, "distance: %d", mavlink_msg_rangefinder_get_distance(&msg));
          //debug_print(LOG_MAV_RANGEFINDER, "distance_CONV: %d", mav.sonarrange);
          //debug_print(LOG_MAV_RANGEFINDER, "voltage: %d", mavlink_msg_rangefinder_get_voltage(&msg));
          logger->add_timestamp(logger->TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER);
          break;
          
        case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
          logger->debug_print(Logger::LOG_MAV_OTHER, (char *)"MAVLINK_MSG_ID_HWSTATUS");          
          rc1 = mavlink_msg_rc_channels_get_chan1_raw(&msg);
          rc2 = mavlink_msg_rc_channels_get_chan2_raw(&msg);
          rc3 = mavlink_msg_rc_channels_get_chan3_raw(&msg);
          rc4 = mavlink_msg_rc_channels_get_chan4_raw(&msg);
          rc5 = mavlink_msg_rc_channels_get_chan5_raw(&msg);
          rc6 = mavlink_msg_rc_channels_get_chan6_raw(&msg);
          rc7 = mavlink_msg_rc_channels_get_chan7_raw(&msg);
          rc8 = mavlink_msg_rc_channels_get_chan8_raw(&msg);
          logger->add_timestamp(logger->TIMESTAMP_MAVLINK_MSG_ID_RC_CHANNELS_RAW);
          break;  
                 
        case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
          break; 
          
        default:
          logger->debug_print(logger->LOG_MAV_UNKNOWN, (char *)"Unhandled MAVLINK message %d", msg.msgid);           
          break;
      }
    }
  }
}


