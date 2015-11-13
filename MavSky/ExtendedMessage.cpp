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
#include "ExtendedMessage.h"
#include "MavLinkData.h"
#include "MavConsole.h"

extern MavLinkData *mav;
extern MavConsole *console;

ExtendedMessage::ExtendedMessage() {
}

void ExtendedMessage::frsky_send_text_message(char *msg) {
  uint8_t c;
  uint16_t dst_index = 0;       
  for(uint16_t i=0; i<strlen(msg); i++) {
    c = msg[i];
    if(c >= 32 && c <= 126) {
      telem_text_message_data_buffer[writing_message_number % TELEM_NUM_BUFFERS][dst_index++] = c;
    }
  }
  telem_text_message_data_buffer[writing_message_number % TELEM_NUM_BUFFERS][dst_index++] = 0;
  writing_message_number++;
}

uint8_t ExtendedMessage::message_available() {
    return reading_message_number != writing_message_number;
}

uint8_t ExtendedMessage::to_six_bit(uint8_t c) {
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

uint8_t ExtendedMessage::get_next_text_byte() {
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

uint16_t ExtendedMessage::telem_next_extension_word() { 
    uint16_t result;
    static uint8_t last_was_text = 0;
    static uint8_t extension_index = 1;

    if(last_was_text == 0 && message_available()) { 
        result = get_next_extension_word(0);
        last_was_text = 1;
    } else {
        result = get_next_extension_word(extension_index++);       
        if(extension_index > 15) { 
            extension_index = 1;  
            message_packet_sequence++;                                         
        }
        last_was_text = 0;
    }
    return result;
}

uint16_t ExtendedMessage::encode_100(float source) {
  uint8_t power;
  if(source >= 10000.0) {                                         // 12,300 -> 123 * 10^2
    if(source > 102300.0) {                                         // largest number is 102,300
      source = 102300.0;
    }
    source = source / 100.0;
    power = 2;
  } else if(source >= 1000.0) {                                   // 1,230 -> 123 * 10^1
    source = source / 10.0;
    power = 1;
  } else if (source >= 100.0) {                                   // 123 -> 123 * 10^0
    source = source;
    power = 0;
  } else {                                                        // 123.0 (or less)  -> 123 * 10^-1
    source = source * 10.0;
    power = 3;
  }
  return (round(source) & 0x3ff) + (power << 10);
}

uint16_t ExtendedMessage::get_next_extension_word(uint8_t extension_command) {                                                     
    uint16_t extension_data = 0;
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
            extension_data = mav->calced_cog;
            break;         
        case 3:
            extension_data = 0;                                // unused
            break;         
        case 4:
            extension_data = mav->gps_hdop / 10;
            break;               
        case 5:
            extension_data = mav->gps_fixtype;
            break;            
        case 6:
            extension_data = mav->gps_satellites_visible;        
            break;  
        case 7:
            extension_data = mav->base_mode;
            break;  
        case 8:
            extension_data = mav->custom_mode;
            break;         
        case 9:
            extension_data = mav->bar_altitude;
            break;  
        case 10:
            extension_data = mav->rangefinder_distance;
            break;    
        case 11:
            extension_data = mav->battery_remaining;
            break;
        case 12:
            extension_data = encode_100((float)mav->current_consumed);
            break;                
        case 13:
            extension_data = mav->armed_distance;       // in m
            break;
        case 14:
            extension_data = encode_100((float)mav->calc_mah_consumed());
            break;   
        case 15:
            extension_data = mav->armed_bearing;        // 0-359
            break;                                         
    }
    if(extension_data > 4095) {
      extension_data = 4095;
    }
    return (extension_command << 12) | extension_data; 
}
