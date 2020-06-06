#ifndef UBLOX_H_
#define UBLOX_H_

//#include "terasic_includes.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include <sys/types.h>
#include <stdint.h>


class UBLOX {
private:
	uint8_t buffer[100];
	uint8_t bufferPVT[100];
	uint8_t bufferRAW[24];
	uint8_t devAddr;

protected:
	uint32_t m_ControllerAddr;
	bool WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue);
	bool writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet=1);
	bool writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value);

	bool readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue);
	bool readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer);

public:
	UBLOX(uint32_t ControllerAddr, uint8_t DeviceAddr = 0xD0);
	virtual ~UBLOX();

	bool getUbloxBuffer();
	bool calcChecksum(uint8_t messageBuffer[], int bufferSize);
	//bool processGPS(NAV_PVT pvt, uint8_t buffer_poll[100]);
	bool getPVT();

	void initialize();
};

#endif /* UBLOX_H_ */
