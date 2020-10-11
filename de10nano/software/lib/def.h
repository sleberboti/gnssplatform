#ifndef DEF_H_
#define DEF_H_

#include <string>
#include <queue>
#include <thread>

struct UBX_RXM_MEASX_REPEATED{
    uint8_t  measx_gnssId;
    uint8_t  measx_svId;
    uint8_t  measx_cNo;
    uint8_t  measx_mpathIndic;
    uint32_t measx_dopplerMS;
    uint32_t measx_dopplerHz;
    uint16_t measx_wholeChips;
    uint16_t measx_fracChips;
    uint32_t measx_codePhase;
    uint8_t  measx_intCodePhase;
    uint8_t  measx_pseuRangeRMSErr;
    uint16_t measx_reserved5;
};

struct sensorPacket {
    std::string sensorname = "";

    bool readPvt;
    bool readMeasx;
    bool readSfrbx;

    // imu_cv5 message
    float acc[3];
    float gyro[3];
    unsigned long long int tow;
    
    // magnetometer message
    int mm[3];
    unsigned long long int mm_itow;

    // ms 5611 altimeter message
    double temperature;
    int pressure;
    unsigned long long int ms5611_itow;

    // imu_icm messages
    float accel_xout;
    float accel_yout;
    float accel_zout;
    float gyro_xout;
    float gyro_yout;
    float gyro_zout;
    float temp_out;
    float mm_xout;
    float mm_yout;
    float mm_zout;
    unsigned long long int icm_itow;

    // ubx pvt messages
    uint32_t iTOW;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t valid;
    uint32_t tAcc;
    int32_t nano;
    uint8_t fixType;
    uint8_t flags;
    uint8_t flags2;
    uint8_t numSV;
    double lon;
    double lat;
    double height;
    double hMSL;
    double hAcc;
    double vAcc;
    double velN;
    double velE;
    double velD;
    double gSpeed;
    double headMot;
    double sAcc;
    double headAcc;
    float pDOP;
    double headVeh;
    float magDec;
    float magAcc;

    uint32_t globitow;

    //ubx-rxm-measx
    uint8_t  version;
    uint32_t gpsTOW;       //[ms]
    uint32_t gloTOW;       //[ms]
    uint32_t bdsTOW;       //[ms]
    uint32_t qzssTOW;      //[ms]
    uint16_t gpsTOWacc;    //[ms]
    uint16_t gloTOWacc;    //[ms]
    uint16_t bdsTOWacc;    //[ms]
    uint16_t qzssTOWacc;   //[ms]
    uint8_t  measx_numSV;
    uint8_t  measx_flags;
    UBX_RXM_MEASX_REPEATED measx_repeated[50];

    //ubx-rxm-sfrbx
    uint8_t  gnssId;
    uint8_t  svId;
    uint8_t  sfrbx_reserved1;
    uint8_t  freqId;
    uint8_t  numWords;    // up to 10
    uint8_t  chn;    
    uint8_t  sfrbx_version;
    uint8_t  sfrbx_reserved2; 
    uint32_t dwrd[20]; // made out of 4 8 bit data each uint32_t dwrd[i] = (b4 << 24) | (b3 << 16) | ( b2 << 8 ) | (b1);
};



#endif