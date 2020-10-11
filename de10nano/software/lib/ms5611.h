#ifndef MS5611_H_
#define MS5611_H_

#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include <sys/types.h>
#include <stdint.h>
#include "def.h"

#define OSR 					2		// 0-3
#define CMD_RESET 				0x1E
#define CMD_ADC_READ			0x00
#define CMD_CONV_D1_BASE 		0x40
#define CMD_CONV_D2_BASE 		0x50
#define CONV_REG_SIZE 			0x02
#define CMD_PROM_READ_BASE		0xA2
#define PROM_REG_SIZE			0x02
#define NBYTES_CONV 			3
#define NBYTES_PROM 			2
#define N_PROM_PARAMS           6
#define T_THR					1000 // temperature sampling threshold in miliseconds

class MS5611 {
private:
	//uint8_t mmData[6];
	//uint8_t regValue[2];
	
    uint8_t     devAddr;  

	sensorPacket packetNow;

    bool        reset();

    int32_t 	_P;
	int32_t  	_T;
	int32_t 	_dT;
	uint16_t 	_C[N_PROM_PARAMS];
	uint32_t 	_lastTime;


protected:
	uint32_t    m_ControllerAddr;
	bool        WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue);
	bool        writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet=1);
	bool        writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value);

	bool        readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue);
	bool        readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer);

public:
	MS5611(uint32_t ControllerAddr, uint8_t DeviceAddr = 0x77);
	virtual ~MS5611();

    //sensorPacket getMSdata();
	//sensorPacket parseMSdata();
     
	bool         initialize();
    uint32_t 	 getRawTemperature();
	int32_t 	 getTemperature();
	uint32_t 	 getRawPressure();
	int32_t 	 getPressure();
	bool 		 readCalibration();
    sensorPacket getMSdata();
    
};

#endif /* MS5611_H_ */