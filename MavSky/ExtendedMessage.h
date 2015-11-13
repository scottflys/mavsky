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
#ifndef EXTENDEDMESSAGE_H
#define EXTENDEDMESSAGE_H

#include <WProgram.h> 


#define TELEM_TEXT_MESSAGE_MAX                     128
#define TELEM_NUM_BUFFERS                           16
  
class ExtendedMessage {
  char telem_text_message_data_buffer[TELEM_NUM_BUFFERS][TELEM_TEXT_MESSAGE_MAX];
  uint8_t telem_text_message_index = 0;
  uint16_t message_packet_sequence = 0;
  uint16_t reading_message_number = 0;
  uint16_t writing_message_number = 0; 
  uint16_t encode_100(float source);
  
  public:
    ExtendedMessage();
    void frsky_send_text_message(char *msg);
    uint8_t message_available();
    uint8_t to_six_bit(uint8_t c);
    uint8_t get_next_text_byte();
    uint16_t telem_next_extension_word();
    uint16_t get_next_extension_word(uint8_t extension_command);    
};

#endif
