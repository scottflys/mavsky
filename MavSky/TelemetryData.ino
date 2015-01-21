
#define EEPROM_INIT_VALUE_110                            0x55
#define EEPROM_INIT_VALUE_111                            0x56

void telem_data_factory_reset() {
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_VFAS, EEPROM_VALUE_MAP_VFAS_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT, EEPROM_VALUE_MAP_CURRENT_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10);    
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED, EEPROM_VALUE_MAP_GPS_SPEED_KPH);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_T2, EEPROM_VALUE_MAP_DATA_T2_BATTERY_REMAINING);
    EEPROM.write(EEPROM_ADDR_HDOP_THRESHOLD, 5);
    EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);
    EEPROM.write(EEPROM_ADDR_HAS_BEEN_INITIALIZED, EEPROM_INIT_VALUE_111);
}

void telem_data_init() {
  if(EEPROM.read(EEPROM_ADDR_HAS_BEEN_INITIALIZED) == EEPROM_INIT_VALUE_110) {                // version 1.1.0
    console_print("Upgrading EEPROM schema from 1.1.0\r\n");
    EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);   
    EEPROM.write(EEPROM_ADDR_HAS_BEEN_INITIALIZED, EEPROM_INIT_VALUE_111); 
  } else if(EEPROM.read(EEPROM_ADDR_HAS_BEEN_INITIALIZED) == EEPROM_INIT_VALUE_111) {         // version 1.1.1
  } else {
    console_print("Resetting EEPROM to default schema\r\n");
    telem_data_factory_reset();
  }
}

uint16_t telem_data_get_value(uint16_t telemetry_data_value_id) {
  switch(telemetry_data_value_id) {
    case TELEM_DATA_VFAS:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_VFAS)) {
        case EEPROM_VALUE_MAP_VFAS_DIRECT:
          return mav.battery_voltage;
          break;
        case EEPROM_VALUE_MAP_VFAS_AVERAGE10:
          return mavlink_get_average(mav.battery_voltage_buffer, mav.battery_voltage_buffer_start, mav.battery_voltage_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_VFAS_AVERAGE50:
          return mavlink_get_average(mav.battery_voltage_buffer, mav.battery_voltage_buffer_start, mav.battery_voltage_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
      }
      break;

    case TELEM_DATA_CURRENT:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT)) {
        case EEPROM_VALUE_MAP_CURRENT_DIRECT:
          return mav.battery_current;
          break;
        case EEPROM_VALUE_MAP_CURRENT_AVERAGE10:
          return mavlink_get_average(mav.battery_current_buffer, mav.battery_current_buffer_start, mav.battery_current_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_CURRENT_AVERAGE50:
          return mavlink_get_average(mav.battery_current_buffer, mav.battery_current_buffer_start, mav.battery_current_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
      }
      break;
 
    case TELEM_DATA_ACCX:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCX)) {
        case EEPROM_VALUE_MAP_ACCX_DIRECT:
          return mav.imu_xacc;
          break;
        case EEPROM_VALUE_MAP_ACCX_AVERAGE10:
          return mavlink_get_average(mav.imu_xacc_buffer, mav.imu_xacc_buffer_start, mav.imu_xacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCX_AVERAGE50:
          return mavlink_get_average(mav.imu_xacc_buffer, mav.imu_xacc_buffer_start, mav.imu_xacc_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10:
          return mavlink_get_average(mav.imu_xacc_peak_buffer, mav.imu_xacc_peak_buffer_start, mav.imu_xacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE50:
          return mavlink_get_average(mav.imu_xacc_peak_buffer, mav.imu_xacc_peak_buffer_start, mav.imu_xacc_peak_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
      }
      break;
     
    case TELEM_DATA_ACCY:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCY)) {
        case EEPROM_VALUE_MAP_ACCY_DIRECT:
          return mav.imu_yacc;
          break;
        case EEPROM_VALUE_MAP_ACCY_AVERAGE10:
          return mavlink_get_average(mav.imu_yacc_buffer, mav.imu_yacc_buffer_start, mav.imu_yacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCY_AVERAGE50:
          return mavlink_get_average(mav.imu_yacc_buffer, mav.imu_yacc_buffer_start, mav.imu_yacc_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10:
          return mavlink_get_average(mav.imu_yacc_peak_buffer, mav.imu_yacc_peak_buffer_start, mav.imu_yacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE50:
          return mavlink_get_average(mav.imu_yacc_peak_buffer, mav.imu_yacc_peak_buffer_start, mav.imu_yacc_peak_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
      }
      break;
     
    case TELEM_DATA_ACCZ:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ)) {
        case EEPROM_VALUE_MAP_ACCZ_DIRECT:
          return mav.imu_zacc;
          break;
        case EEPROM_VALUE_MAP_ACCZ_AVERAGE10:
          return mavlink_get_average(mav.imu_zacc_buffer, mav.imu_zacc_buffer_start, mav.imu_zacc_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCZ_AVERAGE50:
          return mavlink_get_average(mav.imu_zacc_buffer, mav.imu_zacc_buffer_start, mav.imu_zacc_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10:
          return mavlink_get_average(mav.imu_zacc_peak_buffer, mav.imu_zacc_peak_buffer_start, mav.imu_zacc_peak_buffer_length, 10, MAV_HISTORY_BUFFER_SIZE);
          break;
        case EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE50:
          return mavlink_get_average(mav.imu_zacc_peak_buffer, mav.imu_zacc_peak_buffer_start, mav.imu_zacc_peak_buffer_length, 50, MAV_HISTORY_BUFFER_SIZE);
          break;
      }
      break;

    case TELEM_DATA_GPS_SPEED:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED)) {
        case EEPROM_VALUE_MAP_GPS_SPEED_KPH:
          return mav.gps_speed * 36;
          break;
        case EEPROM_VALUE_MAP_GPS_SPEED_MPS:
          return mav.gps_speed * 10;
          break;
      }
      break;
  // temp2          <- battery_remaining, mission_current_seq, temperature, wp_dist  // *configurable

    case TELEM_DATA_T2:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_T2)) {
        case EEPROM_VALUE_MAP_DATA_T2_BATTERY_REMAINING:
          return mav.battery_remaining;
          break;
        case EEPROM_VALUE_MAP_DATA_T2_MISSION_CURRENT_SEQ:
          return mav.mission_current_seq;
          break;
        case EEPROM_VALUE_MAP_DATA_T2_TEMPERATURE:
          return mav.temperature;
          break;
        case EEPROM_VALUE_MAP_DATA_T2_WP_DIST:
          return mav.wp_dist;
          break;
        case EEPROM_VALUE_MAP_DATA_T2_HDOP:
          int16_t hdop_val;
          hdop_val = mav.gps_hdop / 10;
          if(hdop_val > 100) {
            hdop_val = 100;
          }
          return hdop_val;
          break;
      }
      break;
      
      
  }     
}

char telem_text_message_data_buffer[TELEM_NUM_BUFFERS][TELEM_TEXT_MESSAGE_MAX];
uint8_t telem_text_message_index = 0;
uint32_t telem_message_expiry = 0L;
uint16_t message_packet_sequence = 0;
uint16_t current_message_number = 0;
uint16_t next_message_number = 1;

void frsky_send_text_message(char *msg) {
  uint8_t c;
  uint16_t dst_index = 0;
  for(int i=0; i<strlen(msg); i++) {
    c = msg[i];
    if(c >= 32 && c <= 126) {
      telem_text_message_data_buffer[next_message_number % TELEM_NUM_BUFFERS][dst_index++] = c;
    }
  }
  telem_text_message_data_buffer[next_message_number % TELEM_NUM_BUFFERS][dst_index++] = 0;
  next_message_number++;
}

void telem_load_next_buffer() {
  if(millis() > telem_message_expiry) {
    if((current_message_number + 1) < next_message_number) {
      telem_message_expiry = millis() + 2000;
      current_message_number++;
    } else {           
      telem_text_message_data_buffer[current_message_number % TELEM_NUM_BUFFERS][0] = '\0';
      telem_message_expiry = millis();                                                              // No reason to keep blank up for a set time
    }
  }
  telem_text_message_index = 0;  
}

void telem_load_next_bufferNEWWWWWWWWWWWWWWWWWWWWWW() {
  if((current_message_number + 1) < next_message_number) {
    
    current_message_number++;
  } else {           
    telem_text_message_data_buffer[current_message_number % TELEM_NUM_BUFFERS][0] = '\0';
  }
  telem_text_message_index = 0;  
}

char frsky_get_next_message_byte() {
  if(current_message_number == next_message_number) {
    return '\0';
  } else if(telem_text_message_index >= strlen(telem_text_message_data_buffer[current_message_number % TELEM_NUM_BUFFERS])) {
    return '\0';
  } else {
    return telem_text_message_data_buffer[current_message_number % TELEM_NUM_BUFFERS][telem_text_message_index++] & 0x7f;
  }
}

uint16_t telem_text_get_word() {                                                      // LSB is lost so use upper 15 bits
    uint16_t data_word;
    char ch, cl;
    ch = frsky_get_next_message_byte();
    data_word = ch << 8;
    cl = frsky_get_next_message_byte();
    data_word |= cl << 1;
    data_word |= (message_packet_sequence++ & 1) << 15;                                // MSB will change on each telemetry packet so rx knows to update message   
    if(ch == '\0' || cl == '\0') {           
      telem_load_next_buffer();
    }               
    return data_word;    
}

