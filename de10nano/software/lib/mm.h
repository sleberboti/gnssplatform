#ifndef MM_H_
#define MM_H_

//#include "terasic_includes.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include <sys/types.h>
#include <stdint.h>
#include "def.h"

#define mmDeviceID       0x00 // Device ID 
#define mmInfo           0x01 // Information 
#define mmStatus1        0x02 // Data Status 1
#define mmX1             0x03 // Low  byte of X-axis data [7:0]
#define mmX2             0x04 // High byte of X-axis data [7:0]
#define mmY1             0x05 // High byte of Y-axis data [7:0]
#define mmY2             0x06 // High byte of Y-axis data [7:0]
#define mmZ1             0x07 // High byte of Z-axis data [7:0]
#define mmZ2             0x08 // High byte of Z-axis data [7:0]
#define mmStatus2        0x09 // Data Status 2
#define control1         0xA  // Chip Control setting 1 
#define control2         0xB  // Chip Control setting 2
#define temp1            0xC  // Low byte for Temperature data 
#define temp2            0xD  // High byte for Temperature data

class MM {
private:
	uint8_t mmData[6];
	uint8_t regValue[2];
	uint8_t devAddr;  

	sensorPacket packetNow;

protected:
	uint32_t m_ControllerAddr;
	bool WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue);
	bool writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet=1);
	bool writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value);

	bool readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue);
	bool readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer);

public:
	MM(uint32_t ControllerAddr, uint8_t DeviceAddr = 0xE);
	virtual ~MM();

    sensorPacket getMMdata(uint32_t globitow);
	sensorPacket parseMMdata(uint32_t globitow);
	void initialize();
};

#endif /* MM_H_ */
