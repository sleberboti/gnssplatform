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
#include <inttypes.h>
#include <def.h>


class UBLOX {
private:
	uint8_t buffer[1000];
	uint8_t bufferPVT[100];
	//int pvtSize = 100;

	const uint8_t ubxHeader[2] = {0xB5, 0x62};
    const uint8_t ubxNavPvt_msgClass = 0x01;
    const uint8_t ubxNavPvt_msgId = 0x07;
	static const uint16_t ubxNavPvt_msgLen = 96;
	uint8_t ubxNavPvt_buffer[ubxNavPvt_msgLen];
	uint8_t byte;

	uint8_t bufferRAW[24];
	int rawSize = 24;
	uint8_t devAddr;

	uint8_t sop[2] = {0xB5, 0x62};
	uint8_t pvt_sop[2] = {0x01 ,0x07};
	uint8_t rawx_sop[2] = {0x02, 0x15};
	
	bool checksum;


	struct UBX_NAV_PVT {
      uint8_t msg_class;
      uint8_t msg_id;
      uint16_t msg_length;
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
      int32_t lon;
      int32_t lat;
      int32_t height;
      int32_t hMSL;
      uint32_t hAcc;
      uint32_t vAcc;
      int32_t velN;
      int32_t velE;
      int32_t velD;
      int32_t gSpeed;
      int32_t headMot;
      uint32_t sAcc;
      uint32_t headAcc;
      uint16_t pDOP;
      uint8_t reserved[6];
      int32_t headVeh;
      int16_t magDec;
      uint16_t magAcc;
    };
	enum FixType {
      NO_FIX = 0,
      DEAD_RECKONING,
      FIX_2D,
      FIX_3D,
      GNSS_AND_DEAD_RECKONING,
      TIME_ONLY
    };
	enum PowerSaveMode {
      NOT_ACTIVE = 0,
      ENABLED,
      ACQUISITION,
      TRACKING,
      OPTIMIZED_TRACKING,
      INACTIVE
    };
	enum CarrierPhaseStatus {
      NO_SOL = 0,
      FLOAT_SOL,
      FIXED_SOL
    };
	struct UBX_NAV_PVT tempPacket;
	struct UBX_NAV_PVT validPacket;

	sensorPacket packetNow;

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

	sensorPacket getUbloxBuffer();
	sensorPacket reportPVT();
	bool calcChecksum(uint8_t messageBuffer[], int bufferSize);
	//bool processGPS(NAV_PVT pvt, uint8_t buffer_poll[100]);

	void initialize();
};

#endif /* UBLOX_H_ */
