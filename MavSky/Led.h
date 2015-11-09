#ifndef LED_H
#define LED_H

#include <WProgram.h> 
#include <Adafruit_NeoPixel.h>

class LedBulbState {
  public:
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    LedBulbState(uint8_t red_param, uint8_t green_param, uint8_t blue_param);
};

class LedStripState {
  private:
    uint8_t bulb_count = 0;
    LedBulbState* bulbs[8];  

  public:
    void add_bulb(uint8_t red_param, uint8_t green_param, uint8_t blue_param);
    void add_bulbs(uint8_t red_param, uint8_t green_param, uint8_t blue_param, uint8_t count);
    LedStripState();
};

class LedPattern {
  private:
    LedStripState* led_strip_states[25];
    uint8_t   strip_state_count = 0;
    char*     pattern_name;
  public:
    LedPattern(char* name_param);
    void add_strip_state(LedStripState* strip_state_param);
};

class Led {
  private:
    Adafruit_NeoPixel strip; 
    LedPattern* led_patterns[25];
    uint16_t led_pattern_count = 0;  
    uint8_t leds_on_strip = 0;
    int8_t state = 0;
    uint16_t state_count = 0;
    uint8_t  sequence_complete = 0;
    uint8_t previous_pattern = 0;

    void add_pattern(LedPattern* pattern);
    void sequence_all_off();
    void sequence_normal();
    void sequence_landing1();
    void sequence_landing2(uint8_t descending);

  public:
    Led(uint8_t pin, uint8_t led_count);
    void process_200_millisecond();
};

#endif


