#include "ublox.h"
#include "i2c_opencores.h"
#include <iostream>

//#define I2C_DEBUG

UBLOX::UBLOX(uint32_t ControllerAddr, uint8_t DeviceAddr)
{
	m_ControllerAddr = ControllerAddr;
	devAddr = DeviceAddr;
    printf("\n\t[UBLOX] ControllerAddress: %lx\n", (long unsigned int)m_ControllerAddr);
    printf("\t[UBLOX] DeviceAddress: %lx\n", (long unsigned int)devAddr);

    const int ref_clk = 50 * 1000 * 1000; // 50Mhz
    const int i2c_clk = 100 * 1000; // 400KHz -- 100KHz works
    I2C_init(m_ControllerAddr, ref_clk, i2c_clk);
}

UBLOX::~UBLOX() {
	// TODO Auto-generated destructor stub
}

bool UBLOX::calcChecksum(uint8_t messageBuffer[], int bufferSize){

    uint8_t CK_A = 0;
    uint8_t CK_B = 0;
    //printf("CalcChesum started\n");
    for(int i=2; i<bufferSize-2; i++){
        CK_A = CK_A + messageBuffer[i];
        CK_B = CK_B + CK_A;
    }

    if(CK_A == messageBuffer[bufferSize-2] and CK_B == messageBuffer[bufferSize-1]){
        return true;
    }
    else{
        return false;
    }
    //printf("CalcChesum ended\n");
}

bool UBLOX::readLenght(){
    bool bSuccess = true;
    uint8_t data[2];
    
    bSuccess = readBytes(devAddr, 0xFD, 2, data);
    if (bSuccess == false){
        printf("Read error while reading 0xFD\n");
    } 
    else{
        uint16_t length = (((uint16_t)data[0] << 8) + (uint16_t)data[1]);
        printf("Lenght: %" PRIu16 "\n", length);        
        printf("uBlox Length %5d %04X\n", length, length);
    }

    bSuccess = readBytes(devAddr, 0xFE, 2, data);
    if (bSuccess == false){
        printf("Read error while reading 0xFE\n");
    } 
    else{
        uint16_t length = (((uint16_t)data[0] << 8) + (uint16_t)data[1]);
        printf("Lenght: %" PRIu16 "\n", length);        
        printf("uBlox Length %5d %04X\n", length, length);
    }

    return bSuccess;
}

bool UBLOX::getPVT(){
    bool bSuccess=true;
    uint8_t bufferAv[2];
    //poll PVT message: B5 62 01 07 00 00 08 19
    //uint8_t pvtPoll[8] = {0xB5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};
    
    /* for (int i=0; i<8; i++){
        bSuccess = WriteReg (devAddr, 0xFF, pvtPoll[i]);
        printf("%s\n", bSuccess ? "true" : "false");
    } */
    readBytes(devAddr, 0xFE, 2, bufferAv);
    for (int i=0; i<2; i++){
        printf("%x ", bufferAv[i]);
    } 

    printf("\n");
    readBytes(devAddr, 0xFD, 2, bufferAv);
    for (int i=0; i<2; i++){
        printf("%x ", bufferAv[i]);
    } 
    //uint8_t dec =| bufferAv[0];
    //printf("In decimal: %x", dec);
    //uint32_t a = (one[0] << 24) | (one[1] << 16) | (one[2] << 8) | one[3];
 
    printf("getPVT() done\n");
    return bSuccess;
}

bool UBLOX::getUbloxBuffer(){

    bool checkSum = false;
    int bufferSize = sizeof(buffer) / sizeof(buffer[0]);
    
    while (!checkSum){
        readBytes(devAddr, 0xff, bufferSize, buffer); 
        /* printf("Buffer: ");
        for (int i=0; i<bufferSize; i++){
            printf("%x ", buffer[i]);
        } */    
        checkSum = calcChecksum(buffer, bufferSize);
    }

    printf("Buffer: ");
    for (int i=0; i<bufferSize; i++){
        printf("%x ", buffer[i]);
    }
    printf("\n");
    
    return true;
}

bool UBLOX::WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue){
    int ack = 0;
    ack |= I2C_start(m_ControllerAddr, devAddr, 0);
    ack |= I2C_write(m_ControllerAddr, RegIndex, 0);
    ack |= I2C_write(m_ControllerAddr, RegValue, 1);
    if (ack != 0) {
        //#ifdef  I2C_DEBUG
    	    printf("\t[UBLOX] DeviceAddress: %lx, RegIndex: %hhx, RegValue: %hhx\n", (long unsigned int)devAddr, RegIndex, RegValue);
        //#endif
    }
    return ((ack == I2C_ACK)? true: false);
}

bool UBLOX::readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue){
    int ack = 0;
    ack |= I2C_start(m_ControllerAddr, devAddr, 0);
    ack |= I2C_write(m_ControllerAddr, RegIndex, 0); // was 0
    ack |= I2C_start(m_ControllerAddr, devAddr, 1);
    uint16_t i;
    for (i = 0; i < Len; ++i) {
        if (i != Len - 1) {
            pRegValue[i] = I2C_read(m_ControllerAddr, 0);
        } else {
            pRegValue[i] = I2C_read(m_ControllerAddr, 1);
        }
    }
    if (ack != 0) {
        #ifdef  I2C_DEBUG
    	    printf("\t[UBLOX] DeviceAddress: %lx, RegIndex: %hhx\n", devAddr, RegIndex);
        #endif
    }
    return ((ack == I2C_ACK)? true: false);
}

bool UBLOX::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet){
	return writeBits(devAddr, RegIndex, BitIndex, 1, bSet?0x01:0x00);
}

bool UBLOX::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value){
	bool bSuccess;
	uint8_t Data8, Mask, BitMask;
	int i;

	bSuccess = readBytes(devAddr, RegIndex, 1, &Data8);
	if (bSuccess){
		// create mask
		Mask = 0x00;
		BitMask = 0x01 << BitEndIndex;
		for(i=0;i<BitNum;i++){
			Mask |= BitMask;
			BitMask >>= 1;
		}
		Data8 &= ~Mask;
		Data8 |= (Value << (BitEndIndex-BitNum+1)) & Mask;
		bSuccess = WriteReg(devAddr, RegIndex, Data8);
	}
	return bSuccess;
}

bool UBLOX::readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer){
	bool bSuccess;
	uint8_t Mask, BitMask;
	int i;

	bSuccess = readBytes(devAddr, RegIndex, 1, Buffer);
	if (bSuccess){
		// create mask
		Mask = 0x00;
		BitMask = 0x01 << BitEndIndex;
		for(i=0;i<BitNum;i++){
			Mask |= BitMask;
			BitMask >>= 1;
		}
		*Buffer &= Mask;
		*Buffer >>= (BitEndIndex-BitNum+1);
	}

	return bSuccess;
}



void UBLOX::initialize() {
    // setClockSource(MPU6050_CLOCK_INTERNAL);
    // setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
    // setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    // setDLPF(1);
    // setSample_div(9);
    // setIntEnable(1);
    // setSleepEnabled(false); // thanks to Jack Elston for pointing this one out!
}

