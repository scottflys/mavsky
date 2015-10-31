
#define EEPROM_INIT_VALUE_110                            0x55
#define EEPROM_INIT_VALUE_111                            0x56
#define EEPROM_INIT_VALUE_120                            0x57    
#define EEPROM_INIT_VALUE_210                            0x58    

void telem_data_factory_reset() {
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_VFAS, EEPROM_VALUE_MAP_VFAS_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_CURRENT, EEPROM_VALUE_MAP_CURRENT_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCX, EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCY, EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10);
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ACCZ, EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10);    
    EEPROM.write(EEPROM_ADDR_MAP_TELEM_DATA_ALT, EEPROM_VALUE_MAP_DATA_ALT_BARO);   
    EEPROM.write(EEPROM_ADDR_FRSKY_VFAS_ENABLE, 1);
    EEPROM.write(EEPROM_ADDR_HAS_BEEN_INITIALIZED, EEPROM_INIT_VALUE_210);
}

void telem_data_init() {
  if(EEPROM.read(EEPROM_ADDR_HAS_BEEN_INITIALIZED) == EEPROM_INIT_VALUE_210) {         // version 2.1.0
  } else {
    console_print("Resetting EEPROM to default schema\r\n");
    telem_data_factory_reset();
  }
}

uint32_t telem_data_get_value(uint16_t telemetry_data_value_id) {
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
      return mav.gps_speed * 10;
      break;
      
   case TELEM_DATA_ALT:
      switch(EEPROM.read(EEPROM_ADDR_MAP_TELEM_DATA_ALT)) {
        case EEPROM_VALUE_MAP_DATA_ALT_BARO:
          return mav.bar_altitude;
          break;
        case EEPROM_VALUE_MAP_DATA_ALT_RANGEFINDER:
          return mav.rangefinder_distance;
          break;
      }
      break;     
  }     
}

char telem_text_message_data_buffer[TELEM_NUM_BUFFERS][TELEM_TEXT_MESSAGE_MAX];
uint8_t telem_text_message_index = 0;
uint16_t message_packet_sequence = 0;
uint16_t reading_message_number = 0;
uint16_t writing_message_number = 0;

void frsky_send_text_message(char *msg) {
  uint8_t c;
  uint16_t dst_index = 0;       
  for(int i=0; i<strlen(msg); i++) {
    c = msg[i];
    if(c >= 32 && c <= 126) {
      telem_text_message_data_buffer[writing_message_number % TELEM_NUM_BUFFERS][dst_index++] = c;
    }
  }
  telem_text_message_data_buffer[writing_message_number % TELEM_NUM_BUFFERS][dst_index++] = 0;
  writing_message_number++;
}

uint8_t message_available() {
    return reading_message_number != writing_message_number;
}

uint8_t to_six_bit(uint8_t c) {
    c = toupper(c);
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 27;  
    } else if (c >= ' ') {
        return 37;
    } else if (c >= '.') {
        return 38; 
    } else if (c >= ',') {
        return 39; 
    } else if (c >= '=') {
        return 40; 
    } else if (c >= ':') {
        return 41; 
    } else if (c >= '!') {
        return 42; 
    } else if (c >= '-') {
        return 43; 
    } else if (c >= '+') {
        return 44; 
    } else {
        return 63;
    }
}

uint8_t get_next_text_byte() {
  if(reading_message_number == writing_message_number) {
    return 0;
  } else if(telem_text_message_index >= strlen(telem_text_message_data_buffer[reading_message_number % TELEM_NUM_BUFFERS])) {
    reading_message_number++;
    telem_text_message_index = 0;
    return 0;
  } else {
    char c = telem_text_message_data_buffer[reading_message_number % TELEM_NUM_BUFFERS][telem_text_message_index++] & 0x7f;
    return to_six_bit(c);
  }
}

uint16_t telem_next_extension_word() { 
    uint16_t result;
    static uint8_t last_was_text = 0;
    static uint8_t extension_index = 1;

    if(last_was_text == 0 && message_available()) { 
        result = get_next_extension_word(0);
        last_was_text = 1;
    } else {
        result = get_next_extension_word(extension_index++);       
        if(extension_index > 12) { 
            extension_index = 1;  
            message_packet_sequence++;                                         
        }
        last_was_text = 0;
    }
    return result;
}

uint16_t get_next_extension_word(uint8_t extension_command) {                                                     
    uint16_t extension_data;
    uint8_t high_byte;
    uint8_t low_byte;
    switch(extension_command) {
        case 0:
            high_byte = get_next_text_byte();
            if(high_byte == 0) {
                extension_data = (high_byte << 6);
            } else {
                low_byte = get_next_text_byte();
                extension_data = (high_byte << 6) | low_byte;              
            }     
            break;        
        case 1:
            extension_data = message_packet_sequence % 4096;
            break;
        case 2:
            extension_data = mav.roll_angle+180;
            break;         
        case 3:
            extension_data = mav.pitch_angle+180;
            break;         
        case 4:
            extension_data = mav.gps_hdop / 10;
            break;               
        case 5:
            extension_data = mav.gps_fixtype;
            break;            
        case 6:
            extension_data = mav.gps_satellites_visible;        
            break;  
        case 7:
            extension_data = mav.base_mode;
            break;  
        case 8:
            extension_data = mav.custom_mode;
            break;         
        case 9:
            extension_data = mav.bar_altitude;
            break;  
        case 10:
            extension_data = mav.rangefinder_distance;
            break;    
        case 11:
            extension_data = mav.battery_remaining;
            break;
        case 12:
            extension_data = mav.current_consumed;
            break;                
    }
    if(extension_data > 4095) {
      extension_data = 4095;
    }
    return (extension_command << 12) | extension_data; 
}
