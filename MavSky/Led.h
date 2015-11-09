#ifndef LED_H
#define LED_H

#include <WProgram.h> 
#include <Adafruit_NeoPixel.h>
  
class Led {
  private:
    Adafruit_NeoPixel strip; 
    uint8_t leds_on_strip = 0;
    int8_t state = 0;
    uint16_t state_count = 0;
    uint8_t  sequence_complete = 0;
    uint8_t previous_pattern = 0;
    void sequence_all_off();
    void sequence_normal();
    void sequence_landing1();
    void sequence_landing2(uint8_t descending);
    
  public:
    Led(uint8_t pin, uint8_t led_count);
    void process_200_millisecond();
};

#endif


