#include "ms5611.h"
#include "i2c_opencores.h"

#define I2C_DEBUG

MS5611::MS5611(uint32_t ControllerAddr, uint8_t DeviceAddr)
{
	m_ControllerAddr = ControllerAddr;
	devAddr = DeviceAddr;
    printf("\n\t[MS5611] ControllerAddress: %lx\n", (long unsigned int)m_ControllerAddr);
    printf("\t[MS5611] DeviceAddress: %lx\n", (long unsigned int)devAddr);

    const int ref_clk = 50 * 1000 * 1000; // 50Mhz
    const int i2c_clk = 400 * 1000; // 400 Mhz works well
    I2C_init(m_ControllerAddr, ref_clk, i2c_clk);

    _T 		= 0;
	_P 		= 0;
	_lastTime 	= T_THR;
	for(uint8_t k=0; k<N_PROM_PARAMS; k++){
        _C[k]=69;
    } 
}

MS5611::~MS5611() {

}

sensorPacket MS5611::getMSdata(){

    packetNow.sensorname  = "ms";
    packetNow.temperature = double(getTemperature())/100;
    packetNow.pressure    = getPressure();

    return packetNow;
}


bool MS5611::initialize(){
    bool bSuccess = true;
    bSuccess = reset();
    usleep(10000); // wait 10 ms
    readCalibration();
    //getRawPressure();
    //getRawTemperature();
    //getTemperature();
    //getPressure();
    
    if (!bSuccess){
        printf("Error during reset/initialization");
    }

    return bSuccess;
}

bool MS5611::readCalibration(){
    bool bSuccess = true;
    for(uint8_t k=0;k<6;k++){
        //WriteReg(devAddr,CMD_PROM_READ_BASE + k*2,1)
        uint8_t regValue[NBYTES_PROM]; // + k*2,2
        //readBytes(devAddr,0x02,2, regValue);        
        readBytes(devAddr, CMD_PROM_READ_BASE+ k*2, NBYTES_PROM, regValue); 
        //printf("C[%d], %x, %x\n",k, regValue[0], regValue[1]);
        _C[k] = regValue[0] << 8 | regValue[1];
        //printf("C[%d]: %x, %d\n", k, _C[k], _C[k]);
	}
    if(!bSuccess){
        printf("Error during getRawPressure\n");
    }
    return bSuccess;
}

int32_t	MS5611::getPressure(){
	getTemperature(); 		//updates temperature _dT and _T
	uint32_t D1 = getRawPressure();
	
	int64_t OFF  = (int64_t)_C[2-1]*65536 + (int64_t)_C[4-1]*_dT/128;	
	int64_t SENS = (int64_t)_C[1-1]*32768 + (int64_t)_C[3-1]*_dT/256;
	_P = (D1*SENS/2097152 - OFF)/32768;
    //printf("Pressure: %d\n",_P);
	return _P;
}

int32_t MS5611::getTemperature(){
	uint32_t D2; 	
	D2  = getRawTemperature();
	_dT = D2-((uint32_t)_C[5-1] * 256);
	_T = 2000 + ((int64_t)_dT * _C[6-1])/8388608;
    //printf("Temperature: %d\n",_T);
	return _T;
}

uint32_t MS5611::getRawPressure(){
    bool bSuccess = true;

    uint8_t regValue[NBYTES_CONV];
    //printf("Writing register\n");
    bSuccess = WriteReg(devAddr,CMD_CONV_D1_BASE+OSR*CONV_REG_SIZE,0);
    usleep((1+2*OSR)*1000);
    bSuccess = readBytes(devAddr, CMD_ADC_READ, NBYTES_CONV, regValue);

    /* printf("Data size:%d\n", NBYTES_CONV);
    printf("Data from:%x, %d\n", CMD_CONV_D1_BASE+OSR*CONV_REG_SIZE, CMD_CONV_D1_BASE+OSR*CONV_REG_SIZE); */
    uint32_t data;
    /* for (int i=0; i<NBYTES_CONV; i++){
        printf("RegValue: %x %d\n",regValue[i],regValue[i]);
    } */

    data = regValue[0] << 16 | regValue[1] << 8 | regValue[2];
    
    /* for (int i=NBYTES_CONV-1; i>=0; i--){
        data |= ((uint32_t)regValue[i] << (8*i));
    } */
    //printf("\nPressure data %x, %d:\n", data); 
    if(!bSuccess){
        printf("Error during getRawPressure\n");
    }
    return data;
}

uint32_t MS5611::getRawTemperature(){
    bool bSuccess = true;
    uint32_t data;
    uint8_t regValue[NBYTES_CONV];
    //printf("Writing register\n");
    bSuccess = WriteReg(devAddr,CMD_CONV_D2_BASE+OSR*CONV_REG_SIZE,0);
    usleep((1+2*OSR)*1000);
    bSuccess = readBytes(devAddr, CMD_ADC_READ, NBYTES_CONV, regValue);

    data = regValue[0] << 16 | regValue[1] << 8 | regValue[2];
    
    //printf("\nTemp data: %x, %d\n", data); 
    
    if(!bSuccess){
        printf("Error during getRawPressure\n");
    }

    return data;
}

bool MS5611::reset(){
    return WriteReg(devAddr, CMD_RESET, 1);
}

bool MS5611::WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue){
    int ack = 0;
    ack |= I2C_start(m_ControllerAddr, devAddr, 0);
    ack |= I2C_write(m_ControllerAddr, RegIndex, 0);
    ack |= I2C_write(m_ControllerAddr, RegValue, 1);
    if (ack != 0) {
        #ifdef  I2C_DEBUG
    	    printf("\t[MS5611] DeviceAddress: %lx, RegIndex: %hhx, RegValue: %hhx\n", (long unsigned int)devAddr, RegIndex, RegValue);
        #endif
    }
    else
    {
        //printf("\tSuccess\n");
    }
    return ((ack == I2C_ACK)? true: false);
}

bool MS5611::readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue){
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
    	    printf("\t[MS5611] DeviceAddress: %lx, RegIndex: %hhx\n", (long unsigned int)devAddr, RegIndex);
        #endif
    }    
    else
    {
        //printf("\tSuccess\n");
    }
    return ((ack == I2C_ACK)? true: false);
}

bool MS5611::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet){
	return writeBits(devAddr, RegIndex, BitIndex, 1, bSet?0x01:0x00);
}

bool MS5611::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value){
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

bool MS5611::readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer){
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