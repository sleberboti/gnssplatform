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

	const uint8_t ubxHeader[2] = {0xB5, 0x62};
  const uint8_t ubxNavPvt_msgClass = 0x01;
  const uint8_t ubxNavPvt_msgId = 0x07;

  const uint8_t ubxRxmMeasx_msgClass = 0x02;
  const uint8_t ubxRxmMeasx_msgId = 0x14;

  const uint8_t ubxRxmSfrbx_msgClass = 0x02;
  const uint8_t ubxRxmSfrbx_msgId = 0x13;

	static const uint16_t ubxNavPvt_msgLen = 96;
  uint16_t ubxRxmMeasx_msgLen = 2000; // always changing -> 44 + 24*numSV
  uint8_t  ubxRxmSfrbx_msgLen = 48;  // always changing -> 8 + 4*numWords, max=48

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

  struct UBX_RXM_MEASX{
    //ubx-rxm-measx
    uint8_t  measx_msg_class;
    uint8_t  measx_msg_id;
    uint8_t  measx_placeholder[2];
    uint8_t  version;
    uint8_t  reserved11;
    uint8_t  reserved12;
    uint8_t  reserved13;
    uint32_t gpsTOW;       //[ms]
    uint32_t gloTOW;       //[ms]
    uint32_t bdsTOW;       //[ms]
    uint8_t  reserved21;
    uint8_t  reserved22;
    uint8_t  reserved23;
    uint8_t  reserved24;
    uint32_t qzssTOW;      //[ms]
    uint16_t gpsTOWacc;    //[ms]
    uint16_t gloTOWacc;    //[ms]
    uint16_t bdsTOWacc;    //[ms]
    uint8_t  reserved31;
    uint8_t  reserved32;
    uint16_t qzssTOWacc;   //[ms]
    uint8_t  measx_numSV;
    uint8_t  measx_flags;
    uint8_t  reserved41;
    uint8_t  reserved42;
    uint8_t  reserved43;
    uint8_t  reserved44;
    uint8_t  reserved45;
    uint8_t  reserved46;
    uint8_t  reserved47;
    uint8_t  reserved48;
    UBX_RXM_MEASX_REPEATED measx_repeated[50];
    
  };

  struct UBX_RXM_MEASX measx_tempPacket;
  struct UBX_RXM_MEASX measx_validPacket;
  

  struct UBX_RXM_SFRBX{
    //ubx-rxm-sfrbx
    uint8_t  sfrbx_msg_class;
    uint8_t  sfrbx_msg_id;
    uint8_t  placeholder[2];
    uint8_t  gnssId;
    uint8_t  svId;
    uint8_t  sfrbx_reserved1;
    uint8_t  freqId;
    uint8_t  numWords; // up to 10
    uint8_t  chn;    
    uint8_t  sfrbx_version;
    uint8_t  sfrbx_reserved2;    
    uint32_t drwd[20];
  };
  struct UBX_RXM_SFRBX sfrbx_tempPacket;
  struct UBX_RXM_SFRBX sfrbx_validPacket;

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
  sensorPacket getMeasxBuffer();
  sensorPacket getPvtBuffer();
	//sensorPacket
  void reportPVT();
  //sensorPacket 
  void reportSfrbx();
  //sensorPacket
  void reportMeasx();
	bool calcChecksum(uint8_t messageBuffer[], int bufferSize);

};

#endif /* UBLOX_H_ */
