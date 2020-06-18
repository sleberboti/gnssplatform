#include "imu_cv5.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/types.h>

#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"

//#define UART_DEBUG

#define IORD(base, index) (*(((uint32_t *)base) + index))
#define IOWR(base, index, data) (*(((uint32_t *)base) + index) = data)

IMU_CV5::IMU_CV5(uint32_t ControllerAddr)
{
    m_ControllerAddr = ControllerAddr;
    printf("\n\t[IMU_CV5] ControllerAddress: %lx\n", (long unsigned int)m_ControllerAddr);
    m_bInitSuccess = streamConfig();
    // baudrate stb
}

IMU_CV5::~IMU_CV5()
{
}

sensorPacket IMU_CV5::parseIMUdata()
{
    /* for (int i=0; i<28; i++){
        printf("%d: %x\n", i, globdata[i]);
    } */

    // Big endian
    acc_int[0] = globdata[2] << 24 | globdata[3] << 16 | globdata[4] << 8 | globdata[5];
    acc_int[1] = globdata[6] << 24 | globdata[7] << 16 | globdata[8] << 8 | globdata[9];
    acc_int[2] = globdata[10] << 24 | globdata[11] << 16 | globdata[12] << 8 | globdata[13];

    gyro_int[0] = globdata[16] << 24 | globdata[17] << 16 | globdata[18] << 8 | globdata[19];
    gyro_int[1] = globdata[20] << 24 | globdata[21] << 16 | globdata[22] << 8 | globdata[23];
    gyro_int[2] = globdata[24] << 24 | globdata[25] << 16 | globdata[26] << 8 | globdata[27];

    imu_string = "imu: ";    
    imu_string += "acc: ";

    bytesToFloat(acc_int, gyro_int);
    //printf("IMU: ");
    for (int i = 0; i < 3; i++){
        //printf("%0.3f ", acc_float[i]);
        imu_string+= std::to_string(acc_float[i]);
        imu_string+= " ";
        packetNow.acc[i]=acc_float[i];
    }

    imu_string += "gyro: ";
    for (int i = 0; i < 3; i++){
        //printf("%0.3f ", gyro_float[i]);
        imu_string+= std::to_string(gyro_float[i]);
        imu_string+= " ";
        packetNow.gyro[i]=gyro_float[i];
    }
    //printf("\n");
    
    tow_int = globdata[36] << 56 |
              globdata[35] << 48 |
              globdata[34] << 40 |
              globdata[33] << 32 |
              globdata[32] << 24 |
              globdata[31] << 16 |
              globdata[30] <<  8 |
              globdata[29];
    
    uint16_t week = globdata[38] << 8 | globdata[37];

    uint16_t ts_flags[3];
    for (int i=0; i<3; i++){
        ts_flags[i] = globdata[39+i];
    }    
    //printf("\nTow2(uint64_t): %" PRIu64 "\n", tow2);
    tow_double = static_cast<double>(tow_int);
    packetNow.tow=tow_double;
        
    /* printf("Tow: %0.3f\n", tow_double);
    printf("Week: %d\n", week);    
    printf("TsFlags %d %d %d\n", ts_flags[0], ts_flags[1], ts_flags[2]); */
    

    std::string s;
    tow_string = "tow: " + std::to_string(tow_double);
    s = imu_string +  tow_string;

    std::cout <<"String: " << s << std::endl;
    printf("PARSEDATA\n");


    
    printf("Packetnow stuff: %0.3f %0.3f\n", acc_float[0],packetNow.acc[0]);

    //packetNow.acc=;
    //memcpy(acc_float, packetNow.acc, sizeof(acc_float));
    //memcpy(gyro_float, packetNow.gyro, sizeof(gyro_float));


    return packetNow;
}

/* std::string IMU_CV5::parseIMUdata()
{
    //for (int i=0; i<28; i++){
    //    printf("%d: %x\n", i, globdata[i]);
    //}

    // Big endian
    acc_int[0] = globdata[2] << 24 | globdata[3] << 16 | globdata[4] << 8 | globdata[5];
    acc_int[1] = globdata[6] << 24 | globdata[7] << 16 | globdata[8] << 8 | globdata[9];
    acc_int[2] = globdata[10] << 24 | globdata[11] << 16 | globdata[12] << 8 | globdata[13];

    gyro_int[0] = globdata[16] << 24 | globdata[17] << 16 | globdata[18] << 8 | globdata[19];
    gyro_int[1] = globdata[20] << 24 | globdata[21] << 16 | globdata[22] << 8 | globdata[23];
    gyro_int[2] = globdata[24] << 24 | globdata[25] << 16 | globdata[26] << 8 | globdata[27];

    imu_string = "imu: ";    
    imu_string += "acc: ";

    bytesToFloat(acc_int, gyro_int);
    printf("IMU: ");
    for (int i = 0; i < 3; i++){
        printf("%0.3f ", acc_float[i]);
        imu_string+= std::to_string(acc_float[i]);
        imu_string+= " ";
    }

    imu_string += "gyro: ";
    for (int i = 0; i < 3; i++){
        printf("%0.3f ", gyro_float[i]);
        imu_string+= std::to_string(gyro_float[i]);
        imu_string+= " ";
    }
    printf("\n");
    
    tow_int = globdata[36] << 56 |
              globdata[35] << 48 |
              globdata[34] << 40 |
              globdata[33] << 32 |
              globdata[32] << 24 |
              globdata[31] << 16 |
              globdata[30] <<  8 |
              globdata[29];
    
    uint16_t week = globdata[38] << 8 | globdata[37];

    uint16_t ts_flags[3];
    for (int i=0; i<3; i++){
        ts_flags[i] = globdata[39+i];
    }    
    //printf("\nTow2(uint64_t): %" PRIu64 "\n", tow2);
    tow_double = static_cast<double>(tow_int);

    printf("Tow: %0.3f\n", tow_double);
    printf("Week: %d\n", week);    
    printf("TsFlags %d %d %d\n", ts_flags[0], ts_flags[1], ts_flags[2]);

    std::string s;
    tow_string = "tow: " + std::to_string(tow_double);
    s = imu_string +  tow_string;

    std::cout << s;
    return s;
} */

sensorPacket IMU_CV5::pollIMU()
{   
    bool bSuccess = true;

    //printf("Polling IMU data\n");
    len = sizeof(poll_imu_data) / sizeof(poll_imu_data[0]);
    bSuccess = send_imu_uart_data(poll_imu_data, len);
    if(bSuccess == false){
        printf("Unsuccessful imu read\n");
    }
   
    return parseIMUdata();
}


bool IMU_CV5::streamConfig()
{
    bool bSuccess;

    //Continuous Data Command Sequence

    #ifdef UART_DEBUG
    printf("Config baudrate (115200)\n");    
    #endif
    len = sizeof(config_baudrate115200) / sizeof(config_baudrate115200[0]);
    bSuccess = send_imu_uart_data(config_baudrate115200, len);

    #ifdef UART_DEBUG    
    printf("Sending idle command\n");
    #endif
    len = sizeof(idle_mode) / sizeof(idle_mode[0]);
    bSuccess = send_imu_uart_data(idle_mode, len);

    #ifdef UART_DEBUG    
    printf("Configuring the IMU Time Data-stream Format\n");
    #endif
    len = sizeof(config_imu_time_datastream) / sizeof(config_imu_time_datastream[0]);
    bSuccess = send_imu_uart_data(config_imu_time_datastream, len);

    #ifdef UART_DEBUG    
    printf("Saving the IMU Data-stream format\n");
    #endif
    len = sizeof(save_imu_format) / sizeof(save_imu_format[0]);
    bSuccess = send_imu_uart_data(save_imu_format, len);

    #ifdef UART_DEBUG    
    printf("Enabling the IMU Data-stream\n");
    #endif
    len = sizeof(enable_imu_datastream) / sizeof(enable_imu_datastream[0]);
    bSuccess = send_imu_uart_data(enable_imu_datastream, len);

    return bSuccess;
}

//void IMU_CV5::get_imu_uart_data(uint32_t *pdata){
void IMU_CV5::get_imu_uart_data()
{
    IMU_Uart_Read();
}

bool IMU_CV5::send_imu_uart_data(uint8_t command[], int len)
{
    bool bSuccess;

    IMU_Uart_Write(command, len);
    bSuccess = IMU_Uart_Read();
    if (bSuccess == false)
    {
        printf("\n\nError in IMU_Uart_Read()\n\n\n");
    }

    return bSuccess;
}

bool IMU_CV5::IMU_Uart_Write(uint8_t command[], int len)
{
#ifdef UART_DEBUG
    printf("\nWriting:\n");
#endif

    for (int i = 0; i < len; i++)
    {
        IOWR(m_ControllerAddr, 1, command[i]); // was 0
        usleep(5);
#ifdef UART_DEBUG
        printf("\t%d: 0x%02hhx  ", i, command[i]);
//printf("Re-read: %x \n",IORD(m_ControllerAddr, 1));
#endif
        usleep(5);
    }
    return true;
}

bool IMU_CV5::IMU_Uart_Read()
{
#ifdef UART_DEBUG
    printf("\nReading:\n");
#endif

    uint8_t tmp[2] = {0};
    uint8_t mip_packet[1000] = {0};
    int it = 0;

    while (!(tmp[0] == sop[0] && tmp[1] == sop[1]))
    {
        tmp[0] = tmp[1];
        tmp[1] = IORD(m_ControllerAddr, 0);
    }
    //printf("\tStart of Packet's been found: %x, %x\n", tmp[0], tmp[1]);
    mip_packet[0] = tmp[0];
    mip_packet[1] = tmp[1];
    it += 2;

    while (IORD(m_ControllerAddr, 0) == tmp[1]){}
    desc = IORD(m_ControllerAddr, 0);
    mip_packet[it] = desc;
    it++;
    //printf("\tDesc: %x\n", desc);

    while (IORD(m_ControllerAddr, 0) == desc){}
    payload = IORD(m_ControllerAddr, 0);
    mip_packet[it] = payload;
    it++;
    // TODO it can happen that usleep(5) causes uncertainty
    usleep(100);

    uint8_t data[payload];
    for (int i = 0; i < payload; i++)
    {
        data[i] = IORD(m_ControllerAddr, 0);
        mip_packet[it] = data[i];
        it++;
        sampler = 0;

        while (data[i] == IORD(m_ControllerAddr, 0)){
            sampler ++;
            if (sampler > sampler_threshold){
                break;
            }
        }   
        //printf("%d\n", sampler);                    
    }

    while (IORD(m_ControllerAddr, 0) == data[payload - 1]){}
    checksum[0] = IORD(m_ControllerAddr, 0);

    while (IORD(m_ControllerAddr, 0) == checksum[0]){}
    checksum[1] = IORD(m_ControllerAddr, 0);

    // loading data into globaldata
    for (int i = 0; i < payload; i++)
    {
        globdata[i] = data[i];
    }

#ifdef UART_DEBUG
    printf("\tStart of Packet's been found: %x, %x\n", tmp[0], tmp[1]);
    printf("\tDesc: %x\n", desc);
    printf("\tPayload: %x\n", payload);
    for (int i = 0; i < payload; i++)
    {
        if (i%14==0){
            printf("\n");
        }
        printf("\t%d Data: %x\n", i, data[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        printf("\tChecksum: %x\n", checksum[i]);
    }
#endif

    bool check = false;
    check = compareChecksum(checksum, calcChecksum(mip_packet, it));
    return check;
}

bool IMU_CV5::compareChecksum(uint8_t checksum[], uint8_t calced_checksum[])
{
#ifdef UART_DEBUG
    //printf("%x %x %x %x", checksum[0], checksum[1], calced_checksum[0], calced_checksum[1]);
#endif

    if ((calced_checksum[0] == checksum[0]) && (calced_checksum[1] == checksum[1]))
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t *IMU_CV5::calcChecksum(uint8_t data[], int len)
{
    fletcher_checksum[0] = 0;
    fletcher_checksum[1] = 0;
#ifdef UART_DEBUG
    /* printf("Calculating the checksum\n");
    for (int i = 0; i < len; i++)
    {
        printf("\t%d Data: %x\n", i, data[i]);
    } */
#endif

    for (int i = 0; i < len; i++)
    {
        fletcher_checksum[0] += data[i];
        //printf("\tFletcher Checksum: %x\n", fletcher_checksum[0]);
        fletcher_checksum[1] += fletcher_checksum[0];
    }

#ifdef UART_DEBUG
    printf("\n");
    for (int i = 0; i < 2; i++)
    {
        printf("\tFletcher Checksum: %x\n", fletcher_checksum[i]);
        //printf("\tFletcher Checksum: %d\n", fletcher_checksum[i]);
    }
    printf("\n");
#endif

    globCs[0] = fletcher_checksum[0];
    globCs[1] = fletcher_checksum[1];

    return fletcher_checksum;
}


void IMU_CV5::bytesToFloat(uint32_t acc_int[], uint32_t gyro_int[])
{
    for (int i = 0; i < 3; i++)
    {
        val.i = acc_int[i];
        acc_float[i] = val.f;
    }

    for (int i = 0; i < 3; i++)
    {
        val.i = gyro_int[i];
        gyro_float[i] = val.f;
    }
}