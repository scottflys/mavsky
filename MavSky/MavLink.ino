#include "MavLink.h"

void mavlink_init() {
  MAVLINK_SERIAL.begin(57600);
}

#define EXPIRY_MILLIS_MAVLINK_MSG_ID_HEARTBEAT   1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_SYS_STATUS  1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_GPS_RAW_INT 1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_VFR_HUD     1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_RAW_IMU     1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_ATTITUDE    1200
#define EXPIRY_MILLIS_MAVLINK_MSG_ID_RANGEFINDER 1200

#define STATUS_TEXT_MAX 128
char status_text_buffer[STATUS_TEXT_MAX];

void mavlink_average_push(int16_t data, int16_t* p_buffer, int16_t* p_start, int16_t* p_length, int16_t max_length) {
  p_buffer[(*p_start + *p_length) % max_length] = data;
  if(*p_length >= max_length) {
    *p_start = (*p_start + 1) % max_length;
  } else {
    (*p_length)++;
  }
}

int16_t mavlink_get_average(int16_t* p_buffer, int16_t start, int16_t length, int16_t use_samples, int16_t max_length) {
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

uint16_t mavlink_calculate_imu_peak(int16_t current_acc, int16_t* p_previous_acc, int16_t* p_lowest_acc, int16_t* p_highest_acc, int16_t average) {
  if(current_acc < average && *p_previous_acc > average) {                      // going under average
    *p_lowest_acc = current_acc;
  } else if(current_acc > average && *p_previous_acc < average) {               // going over average
    *p_highest_acc = current_acc;
  }
  if (current_acc > average && current_acc > *p_highest_acc) {
    *p_highest_acc = current_acc;
  } else if(current_acc < average && current_acc < *p_lowest_acc) {
    *p_lowest_acc = current_acc;
  }
  *p_previous_acc = current_acc;
  return (*p_highest_acc - *p_lowest_acc) / 2;
}

int mavlink_online() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT) < EXPIRY_MILLIS_MAVLINK_MSG_ID_HEARTBEAT) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_sys_status_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS) < EXPIRY_MILLIS_MAVLINK_MSG_ID_SYS_STATUS) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_gps_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT) < EXPIRY_MILLIS_MAVLINK_MSG_ID_GPS_RAW_INT) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_vfr_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD) < EXPIRY_MILLIS_MAVLINK_MSG_ID_VFR_HUD) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_imu_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU) < EXPIRY_MILLIS_MAVLINK_MSG_ID_RAW_IMU) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_attitude_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE) < EXPIRY_MILLIS_MAVLINK_MSG_ID_ATTITUDE) {
    return 1;
  } else {
    return 0;
  }
}

int mavlink_rangefinder_data_valid() {
  if(get_timestamp_age(TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER) < EXPIRY_MILLIS_MAVLINK_MSG_ID_RANGEFINDER) {
    return 1;
  } else {
    return 0;
  }
}

void process_mavlink_packets() { 
  mavlink_message_t msg;
  mavlink_status_t status;

  while(MAVLINK_SERIAL.available()) { 
    uint8_t c = MAVLINK_SERIAL.read();
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      switch(msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:                            //0
          debug_print(LOG_MAV_HEARTBEAT, "MAVLINK_MSG_ID_HEARTBEAT: base_mode: %d, custom_mode: %d", mavlink_msg_heartbeat_get_base_mode(&msg), mavlink_msg_heartbeat_get_custom_mode(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "MAVLINK_MSG_ID_HEARTBEAT: #0");
          //debug_print(LOG_MAV_HEARTBEAT, "type: %d", mavlink_msg_heartbeat_get_type(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "autopilot: %d", mavlink_msg_heartbeat_get_autopilot(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "base_mode: %d", mavlink_msg_heartbeat_get_base_mode(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "custom_mode: %d", mavlink_msg_heartbeat_get_custom_mode(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "system_status: %d", mavlink_msg_heartbeat_get_system_status(&msg));
          //debug_print(LOG_MAV_HEARTBEAT, "mavlink_version: %d", mavlink_msg_heartbeat_get_mavlink_version(&msg));        
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_HEARTBEAT);
          mav.base_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
          mav.custom_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
          break;
  
        case MAVLINK_MSG_ID_STATUSTEXT:                         //253
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_STATUSTEXT);
          mavlink_msg_statustext_decode(&msg, &mav.statustext);
          sprintf(status_text_buffer, "%d%s", mav.statustext.severity & 0x7, mav.statustext.text);
          frsky_send_text_message(status_text_buffer);          
          debug_print(LOG_MAV_TEXT, "MAVLINK_MSG_ID_STATUSTEXT: severity: %ld text: %s", mav.statustext.severity, mav.statustext.text);
          break;
  
        case MAVLINK_MSG_ID_SYS_STATUS :                       //1
          mav.battery_voltage = max(mavlink_msg_sys_status_get_voltage_battery(&msg), 0);     // 1 = 1mV              // avoid negative values
          mav.battery_current = max(mavlink_msg_sys_status_get_current_battery(&msg), 0);     // 1 = 10mA
          mavlink_average_push(mav.battery_voltage, mav.battery_voltage_buffer, &(mav.battery_voltage_buffer_start), &(mav.battery_voltage_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mavlink_average_push(mav.battery_current, mav.battery_current_buffer, &(mav.battery_current_buffer_start), &(mav.battery_current_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mav.battery_remaining = mavlink_msg_sys_status_get_battery_remaining(&msg); //battery capacity reported in %
          debug_print(LOG_MAV_STATUS, "MAVLINK_MSG_ID_SYS_STATUS: voltage_battery: %d, current_battery: %d", mavlink_msg_sys_status_get_voltage_battery(&msg), mavlink_msg_sys_status_get_current_battery(&msg));
          //debug_print(LOG_MAV_STATUS, "MAVLINK_MSG_ID_SYS_STATUS: #1");
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_present: %d", mavlink_msg_sys_status_get_onboard_control_sensors_present(&msg));
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_enabled: %d", mavlink_msg_sys_status_get_onboard_control_sensors_enabled(&msg));
          //debug_print(LOG_MAV_STATUS, "onboard_control_sensors_health: %d", mavlink_msg_sys_status_get_onboard_control_sensors_health(&msg));
          //debug_print(LOG_MAV_STATUS, "load: %d", mavlink_msg_sys_status_get_load(&msg));
          //debug_print(LOG_MAV_STATUS, "voltage_battery: %d", mavlink_msg_sys_status_get_voltage_battery(&msg));
          //debug_print(LOG_MAV_STATUS, "current_battery: %d", mavlink_msg_sys_status_get_current_battery(&msg));
          //debug_print(LOG_MAV_STATUS, "battery_remaining: %d", mavlink_msg_sys_status_get_battery_remaining(&msg));
          //debug_print(LOG_MAV_STATUS, "drop_rate_comm: %d", mavlink_msg_sys_status_get_drop_rate_comm(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_comm: %d", mavlink_msg_sys_status_get_errors_comm(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count1: %d", mavlink_msg_sys_status_get_errors_count1(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count2: %d", mavlink_msg_sys_status_get_errors_count2(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count3: %d", mavlink_msg_sys_status_get_errors_count3(&msg));
          //debug_print(LOG_MAV_STATUS, "errors_count4: %d", mavlink_msg_sys_status_get_errors_count4(&msg));
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_SYS_STATUS);
          break;
  
        case MAVLINK_MSG_ID_GPS_RAW_INT:                               //24
          mav.gps_fixtype = mavlink_msg_gps_raw_int_get_fix_type(&msg);                              // 0 = No GPS, 1 =No Fix, 2 = 2D Fix, 3 = 3D Fix
          if(mav.gps_fixtype == 3) {
            mav.gps_satellites_visible =  mavlink_msg_gps_raw_int_get_satellites_visible(&msg);      
            mav.gps_hdop = mavlink_msg_gps_raw_int_get_eph(&msg);                                    // hdop * 100
            mav.gps_latitude = mavlink_msg_gps_raw_int_get_lat(&msg);
            mav.gps_longitude = mavlink_msg_gps_raw_int_get_lon(&msg);
            mav.gps_altitude = mavlink_msg_gps_raw_int_get_alt(&msg);                                // 1m =1000
            mav.gps_speed = mavlink_msg_gps_raw_int_get_vel(&msg);                                   // 100 = 1m/s
          } else {
            mav.gps_satellites_visible =  mavlink_msg_gps_raw_int_get_satellites_visible(&msg);      
            mav.gps_hdop = 9999;
            mav.gps_latitude = 0L;
            mav.gps_longitude = 0L;
            mav.gps_altitude = 0L;                       
            mav.gps_speed = 0L;                     
          }
          debug_print(LOG_MAV_GPS, "MAVLINK_MSG_ID_GPS_RAW_INT: fixtype: %d, visiblesats: %d, gpsspeed: %f, hdop: %f, alt: %d", mav.gps_fixtype, mav.gps_satellites_visible, mavlink_msg_gps_raw_int_get_vel(&msg)/100.0, mav.gps_hdop/100.0, mavlink_msg_gps_raw_int_get_alt(&msg));
          //debug_print(LOG_MAV_GPS, "MAVLINK_MSG_ID_GPS_RAW_INT: #24");
          //debug_print(LOG_MAV_GPS, "time_usec: %d", mavlink_msg_gps_raw_int_get_time_usec(&msg));
          //debug_print(LOG_MAV_GPS, "fix_type: %d", mav.gps_fixtype);
          //debug_print(LOG_MAV_GPS, "lat: %d", mav.gps_latitude);
          //debug_print(LOG_MAV_GPS, "lon: %d", mav.gps_longitude);
          //debug_print(LOG_MAV_GPS, "alt: %d", mav.gps_altitude);
          //debug_print(LOG_MAV_GPS, "eph: %d", mavlink_msg_gps_raw_int_get_eph(&msg));
          //debug_print(LOG_MAV_GPS, "epv: %d", mavlink_msg_gps_raw_int_get_epv(&msg));
          //debug_print(LOG_MAV_GPS, "vel: %d", mav.gps_speed);
          //debug_print(LOG_MAV_GPS, "cog: %d", mavlink_msg_gps_raw_int_get_cog(&msg));
          //debug_print(LOG_MAV_GPS, "satellites_visible: %d", mav.gps_satellites_visible);          
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_GPS_RAW_INT);
          break;
  
        case MAVLINK_MSG_ID_RAW_IMU:  //27
          mav.imu_xacc = mavlink_msg_raw_imu_get_xacc(&msg);
          mav.imu_yacc = mavlink_msg_raw_imu_get_yacc(&msg);
          mav.imu_zacc = mavlink_msg_raw_imu_get_zacc(&msg);
          
          uint16_t xacc_average, yacc_average, zacc_average;
          xacc_average = mavlink_get_average(mav.imu_xacc_buffer, mav.imu_xacc_buffer_start, mav.imu_xacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          yacc_average = mavlink_get_average(mav.imu_yacc_buffer, mav.imu_yacc_buffer_start, mav.imu_yacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          zacc_average = mavlink_get_average(mav.imu_zacc_buffer, mav.imu_zacc_buffer_start, mav.imu_zacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          mav.imu_xacc_peak = mavlink_calculate_imu_peak(mav.imu_xacc, &mav.imu_xacc_previous, &mav.imu_xacc_lowest, &mav.imu_xacc_highest, xacc_average);
          mav.imu_yacc_peak = mavlink_calculate_imu_peak(mav.imu_yacc, &mav.imu_yacc_previous, &mav.imu_yacc_lowest, &mav.imu_yacc_highest, yacc_average);
          mav.imu_zacc_peak = mavlink_calculate_imu_peak(mav.imu_zacc, &mav.imu_zacc_previous, &mav.imu_zacc_lowest, &mav.imu_zacc_highest, zacc_average);
          
          mavlink_average_push(mav.imu_xacc, mav.imu_xacc_buffer, &(mav.imu_xacc_buffer_start), &(mav.imu_xacc_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mavlink_average_push(mav.imu_yacc, mav.imu_yacc_buffer, &(mav.imu_yacc_buffer_start), &(mav.imu_yacc_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mavlink_average_push(mav.imu_zacc, mav.imu_zacc_buffer, &(mav.imu_zacc_buffer_start), &(mav.imu_zacc_buffer_length), MAV_HISTORY_BUFFER_SIZE);

          mavlink_average_push(mav.imu_xacc_peak, mav.imu_xacc_peak_buffer, &(mav.imu_xacc_peak_buffer_start), &(mav.imu_xacc_peak_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mavlink_average_push(mav.imu_yacc_peak, mav.imu_yacc_peak_buffer, &(mav.imu_yacc_peak_buffer_start), &(mav.imu_yacc_peak_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          mavlink_average_push(mav.imu_zacc_peak, mav.imu_zacc_peak_buffer, &(mav.imu_zacc_peak_buffer_start), &(mav.imu_zacc_peak_buffer_length), MAV_HISTORY_BUFFER_SIZE);
          
          debug_print(LOG_MAV_IMU, "MAVLINK_MSG_ID_RAW_IMU: xacc: %d, yacc: %d, zacc: %d", mavlink_msg_raw_imu_get_xacc(&msg), mavlink_msg_raw_imu_get_yacc(&msg), mavlink_msg_raw_imu_get_zacc(&msg));
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
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_RAW_IMU);
          break;
  
        case MAVLINK_MSG_ID_ATTITUDE:     //30
         mav.roll_angle = mavlink_msg_attitude_get_roll(&msg)*180/3.1416;  //value comes in rads, convert to deg
          if(abs(mav.roll_angle) <= 90) {
           mav.pitch_angle = mavlink_msg_attitude_get_pitch(&msg)*180/3.1416; //value comes in rads, convert to deg
          } 
          else {
           mav.pitch_angle = 180-mavlink_msg_attitude_get_pitch(&msg)*180/3.1416;   // upside down.  value comes in rads, convert to deg
          }
          mav.yaw_angle = (mavlink_msg_attitude_get_yaw(&msg)+3.1416)*162.9747; //value comes in rads, add pi and scale to 0 to 1024
          debug_print(LOG_MAV_ATTITUDE, "MAVLINK_MSG_ID_ATTITUDE: roll_angle: %f (%d deg), pitch_angle: %f (%d deg), yaw_angle: %f", mavlink_msg_attitude_get_roll(&msg),mav.roll_angle, mavlink_msg_attitude_get_pitch(&msg), mav.pitch_angle, mavlink_msg_attitude_get_yaw(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "MAVLINK_MSG_ID_ATTITUDE: #30");
          //debug_print(LOG_MAV_ATTITUDE, "time_boot_ms: %d", mavlink_msg_attitude_get_time_boot_ms(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "roll: %d", mavlink_msg_attitude_get_roll(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "pitch: %d", mavlink_msg_attitude_get_pitch(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "yaw: %d", mavlink_msg_attitude_get_yaw(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "rollspeed: %d", mavlink_msg_attitude_get_rollspeed(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "pitchspeed: %d", mavlink_msg_attitude_get_pitchspeed(&msg));
          //debug_print(LOG_MAV_ATTITUDE, "yawspeed: %d", mavlink_msg_attitude_get_yawspeed(&msg));
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_ATTITUDE);
          break;
  
        case MAVLINK_MSG_ID_VFR_HUD:                                        //74
          mav.groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);      // 100 = 1m/s
          mav.heading = mavlink_msg_vfr_hud_get_heading(&msg);              // 100 = 100 deg
          mav.throttle = mavlink_msg_vfr_hud_get_throttle(&msg);            //  100 = 100%
          mav.bar_altitude = mavlink_msg_vfr_hud_get_alt(&msg) * 100;       //  m  ====> cm 
          mav.ap_climb_rate=mavlink_msg_vfr_hud_get_climb(&msg) * 100;         //  m/s
          //Modificaciones para muestra de todos los mensajes Mavlink y sus valores
          debug_print(LOG_MAV_VFR, "MAVLINK_MSG_ID_VFR_HUD: groundspeed: %d, heading: %d, throttle: %d, alt: %d, climbrate: %d", mav.groundspeed, mav.heading, mav.throttle, mav.bar_altitude, mav.ap_climb_rate);
          //debug_print(LOG_MAV_VFR, "MAVLINK_MSG_ID_VFR_HUD: #74");
          //debug_print(LOG_MAV_VFR, "altitude: %d", mav.bar_altitude);          
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_VFR_HUD);
          break; 
  
        case MAVLINK_MSG_ID_MISSION_CURRENT:                                  // 42
          mav.mission_current_seq = mavlink_msg_mission_current_get_seq(&msg);
          debug_print(LOG_MAV_OTHER, "MAVLINK MSG_ID_MISSION_CURRENT: #42");
          //debug_print(LOG_MAV_OTHER, "seq: %d", mav.mission_current_seq);     
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_MISSION_CURRENT);
          break;

        case MAVLINK_MSG_ID_SCALED_PRESSURE:                                  // 29
          mav.temperature = mavlink_msg_scaled_pressure_get_temperature(&msg);
          debug_print(LOG_MAV_OTHER, "MAVLINK MSG_ID_SCALED_PRESSURE: #29");
          //debug_print(LOG_MAV_OTHER, "time_boot_ms: %d", mavlink_msg_scaled_pressure_get_time_boot_ms(&msg));
          //debug_print(LOG_MAV_OTHER, "press_abs: %d", mavlink_msg_scaled_pressure_get_press_abs(&msg));
          //debug_print(LOG_MAV_OTHER, "press_diff: %d", mavlink_msg_scaled_pressure_get_press_diff(&msg));
          //debug_print(LOG_MAV_OTHER, "temperature: %d",  mav.temperature);          
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_SCALED_PRESSURE);
          break;

        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:                            // 62
          mav.wp_dist = mavlink_msg_nav_controller_output_get_wp_dist(&msg);
          debug_print(LOG_MAV_OTHER, "MAVLINK MSG_ID_NAV_CONTROLLER_OUTPUT: #62");
          //debug_print(LOG_MAV_OTHER, "nav_roll: %d", mavlink_msg_nav_controller_output_get_nav_roll(&msg));
          //debug_print(LOG_MAV_OTHER, "nav_pitch: %d", mavlink_msg_nav_controller_output_get_nav_pitch(&msg));
          //debug_print(LOG_MAV_OTHER, "nav_bearing: %d", mavlink_msg_nav_controller_output_get_nav_bearing(&msg));
          //debug_print(LOG_MAV_OTHER, "target_bearing: %d", mavlink_msg_nav_controller_output_get_target_bearing(&msg));
          //debug_print(LOG_MAV_OTHER, "wp_dist: %d", mav.wp_dist);
          //debug_print(LOG_MAV_OTHER, "alt_error: %d", mavlink_msg_nav_controller_output_get_alt_error(&msg));
          //debug_print(LOG_MAV_OTHER, "aspd_error: %d", mavlink_msg_nav_controller_output_get_aspd_error(&msg));
          //debug_print(LOG_MAV_OTHER, "xtrack_error: %d", mavlink_msg_nav_controller_output_get_xtrack_error(&msg));  
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_CONTROLLER_OUTPUT);
          break;
          
        case MAVLINK_MSG_ID_SYSTEM_TIME:                                      // 2
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_SYSTEM_TIME: #2");
          //debug_print(LOG_MAV_OTHER, "time_unix_usec: %d", mavlink_msg_system_time_get_time_unix_usec(&msg));
          //debug_print(LOG_MAV_OTHER, "time_boot_ms: %d", mavlink_msg_system_time_get_time_boot_ms(&msg));
          break;
          
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:                              // 33
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_GLOBAL_POSITION_INT: #33");
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

        case MAVLINK_MSG_ID_BATTERY_STATUS:                                 // 147
          mav.current_consumed = mavlink_msg_battery_status_get_current_consumed(&msg);    
          mav.energy_consumed = mavlink_msg_battery_status_get_energy_consumed(&msg);    
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_BATTERY_STATUS: #147");
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
        
        case MAVLINK_MSG_ID_SENSOR_OFFSETS:                                   // 150
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_SENSOR_OFFSETS: #150");
          break;
          
        case MAVLINK_MSG_ID_MEMINFO:                                          // 152
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_MEMINFO: #152");
          //debug_print(LOG_MAV_OTHER, "brkval: %d", mavlink_msg_meminfo_get_brkval(&msg));
          //debug_print(LOG_MAV_OTHER, "freemem: %d", mavlink_msg_meminfo_get_freemem(&msg));
          break;

        case MAVLINK_MSG_ID_AHRS:                                             // 163
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_AHRS: #163");
          break;

        case MAVLINK_MSG_ID_HWSTATUS:                                         // 165
          debug_print(LOG_MAV_OTHER, "MAVLINK_MSG_ID_HWSTATUS: #165");          
          break;

        case MAVLINK_MSG_ID_RANGEFINDER:                                     // 173
          mav.rangefinder_distance = mavlink_msg_rangefinder_get_distance(&msg) * 100;   //RANGEFINDER distance in cm
          debug_print(LOG_MAV_RANGEFINDER, "MAVLINK_MSG_ID_RANGEFINDER: #173");
          //debug_print(LOG_MAV_RANGEFINDER, "distance: %d", mavlink_msg_rangefinder_get_distance(&msg));
          //debug_print(LOG_MAV_RANGEFINDER, "distance_CONV: %d", mav.sonarrange);
          //debug_print(LOG_MAV_RANGEFINDER, "voltage: %d", mavlink_msg_rangefinder_get_voltage(&msg));
          add_timestamp(TIMESTAMP_MAVLINK_MSG_ID_RANGEFINDER);
          break;
          
        default:
          debug_print(LOG_MAV_UNKNOWN, "Unhandled MAVLINK message %d", msg.msgid);           
          break;
      }
    }
  }
}


