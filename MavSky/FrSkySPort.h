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
#ifndef FRSKYSPORT_H
#define FRSKYSPORT_H

#include <WProgram.h> 
#include <GCS_MAVLink.h>

#define SENSOR_ID_VARIO             0x00 // 0
#define SENSOR_ID_FLVSS             0xA1 // 1
#define SENSOR_ID_FAS               0x22 // 2
#define SENSOR_ID_GPS               0x83 // 3
#define SENSOR_ID_RPM               0xE4 // 4
#define SENSOR_ID_SP2UH             0x45 // 5
#define SENSOR_ID_SP2UR             0xC6 // 6

#define START_STOP                  0x7e
#define DATA_FRAME                  0x10

#define FR_ID_ALTITUDE              0x0100  //ALT_FIRST_ID
#define FR_ID_VARIO                 0x0110  //VARIO_FIRST_ID

#define FR_ID_VFAS                  0x0210  //VFAS_FIRST_ID
#define FR_ID_CURRENT               0x0200  //CURR_FIRST_ID               

#define FR_ID_CELLS                 0x0300  //CELLS_FIRST_ID
#define FR_ID_CELLS_LAST            0x030F  //CELLS_LAST_ID

#define FR_ID_T1                    0x0400  //T1_FIRST_ID                   
#define FR_ID_T2                    0x0410  //T2_FIRST_ID                    

#define FR_ID_RPM                   0x0500  //RPM_FIRST_ID

#define FR_ID_FUEL                  0x0600  //FUEL_FIRST_ID

#define FR_ID_ACCX                  0x0700  //ACCX_FIRST_ID
#define FR_ID_ACCY                  0x0710  //ACCY_FIRST_ID
#define FR_ID_ACCZ                  0x0720  //ACCZ_FIRST_ID

#define FR_ID_LATLONG               0x0800  //GPS_LONG_LATI_FIRST_ID
#define FR_ID_GPS_ALT               0x0820  //GPS_ALT_FIRST_ID              
#define FR_ID_SPEED                 0x0830  //GPS_SPEED_FIRST_ID        
#define FR_ID_GPS_COURSE            0x0840  //GPS_COURS_FIRST_ID
#define FR_ID_GPS_TIME_DATE         0x0850  //GPS_TIME_DATE_FIRST_ID

#define FR_ID_A3_FIRST              0x0900  //A3_FIRST_ID
#define FR_ID_A4_FIRST              0x0910  //A4_FIRST_ID

#define FR_ID_AIR_SPEED_FIRST       0x0A00  //AIR_SPEED_FIRST_ID

#define FR_ID_RSSI                  0xF101  //used by the radio system
#define FR_ID_ADC1                  0xF102  //ADC1_ID
#define FR_ID_ADC2                  0xF103  //ADC2_ID                       
#define FR_ID_BATT                  0xF104  // used by the radio system
#define FR_ID_SWR                   0xF105  // used by the radio system

class FrSkySPort {  
  private:
    short crc;                      
    boolean waitingForSensorId = false;
    uint8_t fas_sensor_state = 0;
    uint8_t vario_sensor_state = 0;
    uint8_t gps_sensor_state = 0;   
    int32_t  vario_vertical_speed = 0;
    int32_t  vario_altitude = 0; 
    uint32_t fas_voltage = 0;
    uint32_t fas_current = 0;
    int32_t gps_longitude = 0;
    int32_t gps_latitude = 0;
    int32_t gps_altitude = 0; 
    uint32_t gps_speed = 0; 
    uint32_t gps_heading = 0;  
    uint32_t rpm = 0;    
    uint32_t sp2uh_fuel = 0;
    void (*vario_data_request_function)(int32_t *altitude, int32_t *vertical_speed) = NULL;
    void (*fas_data_request_function)(uint32_t *voltage, uint32_t *current) = NULL;
    void (*gps_data_request_function)(int32_t *lon, int32_t *lat, int32_t *alt, uint32_t *speed, uint32_t *heading) = NULL;  
    void (*rpm_data_request_function)(uint32_t *rpm) = NULL;  
    void (*sp2uh_data_request_function)(uint32_t *fuel) = NULL; 
      
  public:
    FrSkySPort();

    int frsky_online();
    void frsky_process(void);
    void frsky_process_sensor_request(uint8_t sensorId);
    void frsky_send_byte(uint8_t byte);
    void frsky_update_crc(uint8_t byte);
    void frsky_send_crc();
    void frsky_send_package(uint16_t id, uint32_t value);
    void frsky_send_null(uint16_t id);   
    void set_vario_request_callback(void (*callback)(int32_t *altitude, int32_t *vertical_speed));
    void set_fas_request_callback(void (*callback)(uint32_t *voltage, uint32_t *current));
    void set_gps_request_callback(void (*callback)(int32_t *lon, int32_t *lat, int32_t *alt, uint32_t *speed, uint32_t *heading));
    void set_rpm_request_callback(void (*callback)(uint32_t *rpm));
    void set_sp2uh_request_callback(void (*callback)(uint32_t *fuel));
};

#endif
    

