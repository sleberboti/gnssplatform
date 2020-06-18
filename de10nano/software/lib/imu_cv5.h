
#ifndef IMU_CV5_H_
#define IMU_CV5_H_
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include <sys/types.h>
#include <stdint.h>
#include <math.h>       /* pow */
#include <inttypes.h>
#include <iostream>
#include <string> 
#include "def.h"

union IntFloat {
        uint32_t i;
        float f;
};

class IMU_CV5{
private:
    uint32_t gyro_int[3];    
    uint32_t acc_int[3];
    float gyro_float[3];
    float acc_float[3];
    uint64_t tow_int;
    double tow_double;
    std::string imu_string;
    std::string tow_string;

    // payload sampler
    int sampler;
    int sampler_threshold = 250;

    // global variable for checksum
    uint8_t globCs[2];      
    uint8_t globdata[100]; // global data 
    
        
    uint8_t idle_mode[8] = {0x75, 0x65, 0x01, 0x02, 0x02, 0x02, 0xE1, 0xC7};
    uint8_t reset[8]     = {0x75, 0x65, 0x01, 0x02, 0x02, 0x7E, 0x5D, 0x43}; 
    uint8_t resume[8]    = {0x75, 0x65, 0x01, 0x02, 0x02, 0x06, 0xE5, 0xCB};

    uint8_t config_baudrate9600[13]   = {0x75, 0x65, 0x0C, 0x07, 0x07, 0x40, 0x01, 0x00, 0x00, 0x25, 0x80, 0x49, 0x09};
    uint8_t config_baudrate115200[13] = {0x75, 0x65, 0x0C, 0x07, 0x07, 0x40, 0x01, 0x00, 0x01, 0xC2, 0x00, 0xF8, 0xDA}; // 115200
    uint8_t config_baudrate230400[13] = {0x75, 0x65, 0x0C, 0x07, 0x07, 0x40, 0x01, 0x00, 0x03, 0x84, 0x00, 0x2B, 0x50}; // 
    
    // only gyro and acc, without timestamp
    uint8_t config_imu_datastream[16] = {0x75, 0x65,
                                         0x0C, 0x0A, 
                                         0x0A, 0x08, 
                                            0x01, 0x02,
                                            0x04, 0x00, 0x0A,
                                            0x05, 0x00, 0x0A,
                                         0x22, 0xA0};

    uint8_t config_imu_time_datastream[19] = {0x75, 0x65,
                                              0x0C, 0x0D, 
                                              0x0D, 0x08, 
                                                0x01, 0x03,
                                                0x04, 0x00, 0x0A,
                                                0x05, 0x00, 0x0A,
                                                0x12, 0x00, 0x0A,
                                              0x45, 0xA1}; // 45 F2

    
    uint8_t save_imu_format[10] = {0x75, 0x65, 0x0C, 0x04, 0x04, 0x08, 0x03, 0x00, 0xF9, 0xF5};

    uint8_t enable_imu_datastream[11] = {0x75, 0x65, 0x0C, 0x05, 0x05, 0x11, 0x01, 0x01, 0x01, 0x04, 0x1A};
    

    uint8_t poll_imu_data[10] = {0x75, 0x65, 0x0C, 0x04, 0x04, 0x01, 0x00, 0x00, 0xEF, 0xDA};
    //                                     1     2      3     4     5    6     7     8     9     10    10    12    13    14   15    16    17
    //uint8_t calcChecksumToBeCalced[17] = {0x75, 0x65, 0x0C, 0x0A, 0x0A, 0x01, 0x00, 0x03, 0x04, 0x00, 0x0A, 0x05, 0x00, 0x0A, 0x12, 0x00, 0x0A};
    uint8_t calcChecksumToBeCalced[18] = {0x75, 0x65, 0x80, 0x0E, 0x0E, 0x04, 0x3E, 0x7A, 0x63, 0xA0, 0xBB, 0x8E, 0x3B, 0x29, 0x7F, 0xE5, 0xBF, 0x7F};
    //uint8_t poll_imu_data2[19] =         {0x75, 0x65, 0x0C, 0x0A, 0x0A, 0x01, 0x00, 0x03, 0x04, 0x00, 0x0A, 0x05, 0x00, 0x0A, 0x12, 0x00, 0x0A, 0x37, 0xF1};
    //uint8_t poll_imu_data2[19] =       {0x75, 0x65, 0x0C, 0x0A, 0x0A, 0x01, 0x00, 0x03, 0x04, 0x00, 0x0A, 0x05, 0x0A, 0x00, 0x12, 0x00, 0x0A, 0x37, 0xF1};

//**************************************/////////
    //uint8_t config_imu_datastream[19] = {0x75, 0x65, 0x0C, 0x0D, 0x0D, 0x08, 0x01, 0x03, 0x12, 0x00, 0x0A, 0x04, 0x00, 0x0A, 0x05, 0x00, 0x0A, 0x45, 0xF2};
    //                                          75    65    0C    10    10    0A    01    04    11    00    0A    05    00    0A    0D    00    0A    0E    00    0A    6E    B0
    uint8_t config_estfilter_datastream[22] = {0x75, 0x65, 0x0C, 0x10, 0x10, 0x0A, 0x01, 0x04, 0x11, 0x00, 0x0A, 0x05, 0x00, 0x0A, 0x0D, 0x00, 0x0A, 0x0E, 0x00, 0x0A, 0x6E, 0xB0};
    // 7565 0C08 0408 0300 040A 0300 0E31
    //only imu part of it
    //                                               75    65    0C    0A    05    11    01    01    01    05    11    01    03    01    24    CC
    uint8_t enable_imu_estfilter_datastream[16] = {0x75, 0x65, 0x0C, 0x0A, 0x05, 0x11, 0x01, 0x01, 0x01, 0x05, 0x11, 0x01, 0x03, 0x01, 0x24, 0xCC};
    //                              75    65    0C    07    07    01    00    01    0C    00    00    02    FC
    uint8_t poll_euler_angle[13] = {0x75, 0x65, 0x0C, 0x07, 0x07, 0x01, 0x00, 0x01, 0x0C, 0x00, 0x00, 0x02, 0xFC};
    //uint8_t poll_euler_angle[12] = {0x75, 0x65, 0x0C, 0x07, 0x07, 0x01, 0x00, 0x00, 0x06, 0x00, 0x02, 0xFC};
    //uint8_t save_imu_estfilter_format[14] = {0x75, 0x65, 0x0C, 0x08, 0x04, 0x08, 0x03, 0x00, 0x04, 0x0A, 0x03, 0x00, 0x0E, 0x31};

//********************************************///   
    

    uint8_t sop[2] = {0x75, 0x65}; // Start Of Packet: Sync bytes, idetifies the start of the packet, same for every MIP packet
    uint8_t desc;
    uint8_t payload;
    uint8_t field_lenght;
    uint8_t checksum[2];
    uint8_t fletcher_checksum[2];
    
    int len;
    union IntFloat val;    
    sensorPacket packetNow;

protected:
	uint32_t m_ControllerAddr;
    bool     m_bInitSuccess;

public:
	IMU_CV5(uint32_t ControllerAddr);
	virtual ~IMU_CV5();

    bool streamConfig();
    void get_imu_uart_data();
    bool send_imu_uart_data(uint8_t command[], int len);
    bool IMU_Uart_Write(uint8_t command[], int len);
    bool IMU_Uart_Read();
    uint8_t * calcChecksum(uint8_t data[], int len);
    bool compareChecksum(uint8_t checksum[], uint8_t calced_checksum[]);

    sensorPacket pollIMU();  
    sensorPacket parseIMUdata();
    void bytesToFloat(uint32_t gyro_int[], uint32_t acc_int[]);

};

#endif /* IMU_CV5_H_ */