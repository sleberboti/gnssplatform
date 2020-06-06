#include "mm.h"
#include "i2c_opencores.h"

//#define I2C_DEBUG

MM::MM(uint32_t ControllerAddr, uint8_t DeviceAddr)
{
	m_ControllerAddr = ControllerAddr;
	devAddr = DeviceAddr;
    printf("\t[UBLOX] ControllerAddress: %lx\n", m_ControllerAddr);
    printf("\t[UBLOX] DeviceAddress: %lx\n", devAddr);

    const int ref_clk = 50 * 1000 * 1000; // 50Mhz
    const int i2c_clk = 400 * 1000; // 400 Mhz works well
    I2C_init(m_ControllerAddr, ref_clk, i2c_clk);
}

MM::~MM() {

}

void MM::initialize() {
    #ifdef  I2C_DEBUG
    readBytes(devAddr, control1, 2, regValue); 
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    #endif
    //WriteReg (devAddr, control1, 0xB); // Continous measurement
    WriteReg (devAddr, control1, 0x1);   // Single measurement
    #ifdef  I2C_DEBUG
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    readBytes(devAddr, control1, 2, regValue);
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    readBytes(devAddr, control2, 2, regValue); 
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    #endif
    WriteReg (devAddr, control2, 0x6);
    #ifdef  I2C_DEBUG
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    readBytes(devAddr, control2, 2, regValue); 
    printf("\tRegValue: %x, %x \n", regValue[0], regValue[1]);
    #endif
}

uint8_t MM::getMMdata(){
    readBytes(devAddr, mmX1, 1, regValue);
    printf("MM registers:\n");
    printf("\tX:  %x   ", regValue[0]);
    mmData[0] = regValue[0];
    readBytes(devAddr, mmX2, 1, regValue);
    printf("%x\n", regValue[0]);
    mmData[0] = regValue[1];
    readBytes(devAddr, mmY1, 1, regValue);
    printf("\tY:  %x   ", regValue[0]);
    mmData[0] = regValue[2];
    readBytes(devAddr, mmY2, 1, regValue);
    printf("%x\n", regValue[0]);
    mmData[0] = regValue[3];
    readBytes(devAddr, mmZ1, 1, regValue);
    printf("\tZ:  %x   ", regValue[0]);
    mmData[0] = regValue[4];
    readBytes(devAddr, mmZ2, 1, regValue);
    printf("%x\n", regValue[0]);
    mmData[0] = regValue[5];

    return mmData[6];
}

bool MM::WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue){
    int ack = 0;
    ack |= I2C_start(m_ControllerAddr, devAddr, 0);
    ack |= I2C_write(m_ControllerAddr, RegIndex, 0);
    ack |= I2C_write(m_ControllerAddr, RegValue, 1);
    if (ack != 0) {
        #ifdef  I2C_DEBUG
    	    printf("\t[UBLOX] DeviceAddress: %lx, RegIndex: %hhx, RegValue: %hhx\n", devAddr, RegIndex, RegValue);
        #endif
    }
    return ((ack == I2C_ACK)? true: false);
}

bool MM::readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue){
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

bool MM::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet){
	return writeBits(devAddr, RegIndex, BitIndex, 1, bSet?0x01:0x00);
}

bool MM::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value){
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

bool MM::readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer){
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