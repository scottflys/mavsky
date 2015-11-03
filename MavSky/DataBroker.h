#ifndef DATABROKER_H
#define DATABROKER_H

#include <WProgram.h> 
#include "ExtendedMessage.h"
#include "DataMap.h" 
 
class DataBroker {
  private:
    
  public:
    DataBroker();
    static void get_vario_data(uint32_t*, uint32_t*);
    static void get_fas_data(uint32_t *p1, uint32_t *p2);
    static void get_gps_data(uint32_t *lon, uint32_t *lat, uint32_t *alt, uint32_t *speed, uint32_t *heading);
    static void get_rpm_data(uint32_t *rpm);
    static void get_sp2uh_data(uint32_t *fuel);
    static ExtendedMessage extended_message_processor;
    static DataMap data_map;
    void write_factory_settings();
    void console_map(char*);
};

// todo try to deprecate following
#define TELEM_TEXT_MESSAGE_MAX                     128
#define TELEM_NUM_BUFFERS                           16

#define EEPROM_ADDR_HAS_BEEN_INITIALIZED             0
#define EEPROM_ADDR_MAP_TELEM_DATA_VFAS              1
#define EEPROM_ADDR_MAP_TELEM_DATA_CURRENT           2
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCX              3
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCY              4
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCZ              5
#define EEPROM_ADDR_FRSKY_VFAS_ENABLE                6
#define EEPROM_ADDR_MAP_TELEM_DATA_ALT               7    


#define EEPROM_VALUE_MAP_ACCX_DIRECT                 1
#define EEPROM_VALUE_MAP_ACCX_AVERAGE10              2
#define EEPROM_VALUE_MAP_ACCX_AVERAGE50              3
#define EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE10         4
#define EEPROM_VALUE_MAP_ACCX_PEAK_AVERAGE50         5

#define EEPROM_VALUE_MAP_ACCY_DIRECT                 1
#define EEPROM_VALUE_MAP_ACCY_AVERAGE10              2
#define EEPROM_VALUE_MAP_ACCY_AVERAGE50              3
#define EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE10         4
#define EEPROM_VALUE_MAP_ACCY_PEAK_AVERAGE50         5

#define EEPROM_VALUE_MAP_ACCZ_DIRECT                 1
#define EEPROM_VALUE_MAP_ACCZ_AVERAGE10              2
#define EEPROM_VALUE_MAP_ACCZ_AVERAGE50              3
#define EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE10         4
#define EEPROM_VALUE_MAP_ACCZ_PEAK_AVERAGE50         5

#define EEPROM_VALUE_MAP_DATA_ALT_BARO               1
#define EEPROM_VALUE_MAP_DATA_ALT_RANGEFINDER        2

#endif


