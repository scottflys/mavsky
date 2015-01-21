#ifndef TELEMETRY_DATA
#define TELEMETRY_DATA


#define TELEM_TEXT_MESSAGE_MAX                     128
#define TELEM_NUM_BUFFERS                            8

#define TELEM_DATA_VFAS                              1
#define TELEM_DATA_CURRENT                           2
#define TELEM_DATA_ACCX                              3
#define TELEM_DATA_ACCY                              4
#define TELEM_DATA_ACCZ                              5
#define TELEM_DATA_GPS_SPEED                         6
#define TELEM_DATA_T2                                7

#define EEPROM_ADDR_HAS_BEEN_INITIALIZED             0
#define EEPROM_ADDR_MAP_TELEM_DATA_VFAS              1
#define EEPROM_ADDR_MAP_TELEM_DATA_CURRENT           2
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCX              3
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCY              4
#define EEPROM_ADDR_MAP_TELEM_DATA_ACCZ              5
#define EEPROM_ADDR_MAP_TELEM_DATA_GPS_SPEED         6
#define EEPROM_ADDR_MAP_TELEM_DATA_T2                7
#define EEPROM_ADDR_HDOP_THRESHOLD                   8
#define EEPROM_ADDR_FRSKY_VFAS_ENABLE                9



#define EEPROM_VALUE_MAP_VFAS_DIRECT                 1
#define EEPROM_VALUE_MAP_VFAS_AVERAGE10              2
#define EEPROM_VALUE_MAP_VFAS_AVERAGE50              3

#define EEPROM_VALUE_MAP_CURRENT_DIRECT              1
#define EEPROM_VALUE_MAP_CURRENT_AVERAGE10           2
#define EEPROM_VALUE_MAP_CURRENT_AVERAGE50           3

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

#define EEPROM_VALUE_MAP_GPS_SPEED_KPH               1
#define EEPROM_VALUE_MAP_GPS_SPEED_MPS               2

#define EEPROM_VALUE_MAP_DATA_T2_BATTERY_REMAINING   1
#define EEPROM_VALUE_MAP_DATA_T2_MISSION_CURRENT_SEQ 2
#define EEPROM_VALUE_MAP_DATA_T2_TEMPERATURE         3
#define EEPROM_VALUE_MAP_DATA_T2_WP_DIST             4
#define EEPROM_VALUE_MAP_DATA_T2_HDOP                5

#endif
