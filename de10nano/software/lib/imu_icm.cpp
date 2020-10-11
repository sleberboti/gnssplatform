#include "imu_icm.h"
#include "i2c_opencores.h"

#define I2C_DEBUG

IMU_ICM::IMU_ICM(uint32_t ControllerAddr, uint8_t DeviceAddr)
{
	m_ControllerAddr = ControllerAddr;
	devAddr = DeviceAddr;
    printf("\n\t[IMU_ICM] ControllerAddress: %lx\n", (long unsigned int)m_ControllerAddr);
    printf("\t[IMU_ICM] DeviceAddress: %lx\n", (long unsigned int)devAddr);

    const int ref_clk = 50 * 1000 * 1000; // 50Mhz
    const int i2c_clk = 400 * 1000; // 400 Mhz works well
    I2C_init(m_ControllerAddr, ref_clk, i2c_clk);
}

IMU_ICM::~IMU_ICM() {

}

sensorPacket IMU_ICM::getICMdata(){

    readSensor();
    packetNow.sensorname = "imu_icm";
    packetNow.accel_xout = getAccelX_mss();
    packetNow.accel_yout = getAccelY_mss();
    packetNow.accel_zout = getAccelZ_mss();
    packetNow.gyro_xout  = getGyroX_rads();
    packetNow.gyro_yout  = getGyroY_rads();
    packetNow.gyro_zout  = getGyroZ_rads(); 
    packetNow.mm_xout    = getMagX_uT();
    packetNow.mm_yout    = getMagY_uT();
    packetNow.mm_zout    = getMagZ_uT();
    packetNow.temp_out   = getTemperature_C();

    return packetNow;
}


int IMU_ICM::initialize(){
    
    if (changeUserBank(USER_BANK_0, true) < 0) { // Make sure that the user bank selection is in sync
        printf("Error duing changing user bank\n");
  	    return -1;
    }
    if (selectAutoClockSource() < 0) {
        return -1;
    }
    // enable I2C master mode
    if(enableI2cMaster() < 0){
        return -2;
    }
    if (powerDownMag() < 0) {
  	    return -3;
    }
    printf("PowerDownMag:%d", powerDownMag());
    reset();
    usleep(1000);// wait for ICM-20948 to come back up
    resetMag(); // Don't check return value as a reset clears the register and can't be verified.
    if (selectAutoClockSource() < 0) {
        return -6;
    }
    if (whoAmI() != ICM20948_WHO_AM_I) {
        return -7;
    }
    if (enableAccelGyro() < 0) {
        return -8;
    }
    if (configAccel(ACCEL_RANGE_16G, ACCEL_DLPF_BANDWIDTH_246HZ) < 0) {
        return -9;
    }
    if (configGyro(GYRO_RANGE_2000DPS, GYRO_DLPF_BANDWIDTH_197HZ) < 0) {
        return -10;
    }
    if (setGyroSrd(0) < 0) { 
        return -11;
    } 
    if (setAccelSrd(0) < 0) { 
        return -12;
    }
    if(enableI2cMaster() < 0) {
        return -13;
    }
	if(whoAmIMag() != MAG_AK09916_WHO_AM_I ) {
        return -14;
	}
    if(configMag() < 0){
        return -18;
    }
    if(selectAutoClockSource() < 0) { // TODO: Why do this again here?
        return -19;
    }

    return 1;
}

/* returns the accelerometer measurement in the x direction, m/s/s */
float IMU_ICM::getAccelX_mss() {
  return _ax;
}

/* returns the accelerometer measurement in the y direction, m/s/s */
float IMU_ICM::getAccelY_mss() {
  return _ay;
}

/* returns the accelerometer measurement in the z direction, m/s/s */
float IMU_ICM::getAccelZ_mss() {
  return _az;
}

/* returns the gyroscope measurement in the x direction, rad/s */
float IMU_ICM::getGyroX_rads() {
  return _gx;
}

/* returns the gyroscope measurement in the y direction, rad/s */
float IMU_ICM::getGyroY_rads() {
  return _gy;
}

/* returns the gyroscope measurement in the z direction, rad/s */
float IMU_ICM::getGyroZ_rads() {
  return _gz;
}

/* returns the magnetometer measurement in the x direction, uT */
float IMU_ICM::getMagX_uT() {
  return _hx;
}

/* returns the magnetometer measurement in the y direction, uT */
float IMU_ICM::getMagY_uT() {
  return _hy;
}

/* returns the magnetometer measurement in the z direction, uT */
float IMU_ICM::getMagZ_uT() {
  return _hz;
}

/* returns the die temperature, C */
float IMU_ICM::getTemperature_C() {
  return _t;
}

int IMU_ICM::readSensor() {
	if (changeUserBank(USER_BANK_0) < 0) {
  	    return -1;
    }
    if (readBytes(devAddr, UB0_ACCEL_XOUT_H, 20, _buffer) < 0) {
        return -1;
    }
    // combine into 16 bit values
    _axcounts = (((int16_t)_buffer[0]) << 8) | _buffer[1];  
    _aycounts = (((int16_t)_buffer[2]) << 8) | _buffer[3];
    _azcounts = (((int16_t)_buffer[4]) << 8) | _buffer[5];
    _gxcounts = (((int16_t)_buffer[6]) << 8) | _buffer[7];
    _gycounts = (((int16_t)_buffer[8]) << 8) | _buffer[9];
    _gzcounts = (((int16_t)_buffer[10]) << 8) | _buffer[11];
    _tcounts = (((int16_t)_buffer[12]) << 8) | _buffer[13];
    _hxcounts = (((int16_t)_buffer[15]) << 8) | _buffer[14];
    _hycounts = (((int16_t)_buffer[17]) << 8) | _buffer[16];
    _hzcounts = (((int16_t)_buffer[19]) << 8) | _buffer[18];
    // transform and convert to float values
    _ax = (((float)_axcounts * _accelScale) - _axb)*_axs;
    _ay = (((float)_aycounts * _accelScale) - _ayb)*_ays;
    _az = (((float)_azcounts * _accelScale) - _azb)*_azs;
    _gx = ((float)_gxcounts * _gyroScale) - _gxb;
    _gy = ((float)_gycounts * _gyroScale) - _gyb;
    _gz = ((float)_gzcounts * _gyroScale) - _gzb;
    _t = ((((float) _tcounts) - _tempOffset)/_tempScale) + _tempOffset;
    _hx = (((float)(tX[0]*_hxcounts + tX[1]*_hycounts + tX[2]*_hzcounts) * _magScale) - _hxb)*_hxs;
    _hy = (((float)(tY[0]*_hxcounts + tY[1]*_hycounts + tY[2]*_hzcounts) * _magScale) - _hyb)*_hys;
    _hz = (((float)(tZ[0]*_hxcounts + tZ[1]*_hycounts + tZ[2]*_hzcounts) * _magScale) - _hzb)*_hzs;
    return 1;
}

int IMU_ICM::configMag() { 
	if (writeMagRegister(MAG_CNTL2, MAG_CNTL2_MODE_100HZ) < 0) {
        //printf("configMag error: %d\n",writeMagRegister(MAG_CNTL2, MAG_CNTL2_MODE_100HZ));
		return -1;
	}
	return 1;
}

int IMU_ICM::setAccelSrd(uint16_t srd) {
	if (changeUserBank(USER_BANK_2) < 0) {
  	return -1;
  }
  uint8_t srdHigh = srd >> 8 & 0x0F; // Only last 4 bits can be set
  if (WriteReg(devAddr, UB2_ACCEL_SMPLRT_DIV_1, srdHigh) == false) {
  	return -1;
	}
	uint8_t srdLow = srd & 0x0F; // Only last 4 bits can be set
  if (WriteReg(devAddr, UB2_ACCEL_SMPLRT_DIV_2, srdLow) == false) {
  	return -1;
	}
	_accelSrd = srd;
	return 1;
}

int IMU_ICM::setGyroSrd(uint8_t srd) {
	if (changeUserBank(USER_BANK_2) < 0 || WriteReg(devAddr, UB2_GYRO_SMPLRT_DIV, srd) == false) {
  	return -1;
	}
	_gyroSrd = srd;
	return 1;
}

/* sets the gyro full scale range to values other than default */
int IMU_ICM::configGyro(GyroRange range, GyroDlpfBandwidth bandwidth) {
    if (changeUserBank(USER_BANK_2) < 0) {
        return -1;
    }
    uint8_t gyroConfigRegValue = 0x00;
    float gyroScale = 0x00;
    switch(range) {
        case GYRO_RANGE_250DPS: {
            gyroConfigRegValue = UB2_GYRO_CONFIG_1_FS_SEL_250DPS;
        gyroScale = 250.0f/gyroRawScaling * _d2r; // setting the gyro scale to 250DPS
        break;
        }
        case GYRO_RANGE_500DPS: {
        gyroConfigRegValue = UB2_GYRO_CONFIG_1_FS_SEL_500DPS;
        gyroScale = 500.0f/gyroRawScaling * _d2r; // setting the gyro scale to 500DPS
        break;  
        }
        case GYRO_RANGE_1000DPS: {
        gyroConfigRegValue = UB2_GYRO_CONFIG_1_FS_SEL_1000DPS;
        gyroScale = 1000.0f/gyroRawScaling * _d2r; // setting the gyro scale to 1000DPS
        break;
        }
        case GYRO_RANGE_2000DPS: {
        gyroConfigRegValue = UB2_GYRO_CONFIG_1_FS_SEL_2000DPS;
        gyroScale = 2000.0f/gyroRawScaling * _d2r; // setting the gyro scale to 2000DPS
        break;
        }
    }
    uint8_t dlpfRegValue = 0x00;
    switch(bandwidth) {
        case GYRO_DLPF_BANDWIDTH_12106HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_12106HZ; break;
        case GYRO_DLPF_BANDWIDTH_197HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_197HZ; break;
        case GYRO_DLPF_BANDWIDTH_152HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_152HZ; break;
        case GYRO_DLPF_BANDWIDTH_120HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_120HZ; break;
        case GYRO_DLPF_BANDWIDTH_51HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_51HZ; break;
        case GYRO_DLPF_BANDWIDTH_24HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_24HZ; break;
        case GYRO_DLPF_BANDWIDTH_12HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_12HZ; break;
        case GYRO_DLPF_BANDWIDTH_6HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_6HZ; break;
        case GYRO_DLPF_BANDWIDTH_361HZ: dlpfRegValue = UB2_GYRO_CONFIG_1_DLPFCFG_361HZ; break;
    }
    if (WriteReg(devAddr, UB2_GYRO_CONFIG_1, gyroConfigRegValue | dlpfRegValue) == false) {
        return -1;
        }
    _gyroScale = gyroScale;
    _gyroRange = range;
    _gyroBandwidth = bandwidth;
    return 1;
}

int IMU_ICM::configAccel(AccelRange range, AccelDlpfBandwidth bandwidth) {
	if (changeUserBank(USER_BANK_2) < 0) {
  	    return -1;
    }
    uint8_t accelRangeRegValue = 0x00;
    float accelScale = 0.0f;
    switch(range) {
        case ACCEL_RANGE_2G: {
        accelRangeRegValue = UB2_ACCEL_CONFIG_FS_SEL_2G;
        accelScale = G * 2.0f/accRawScaling; // setting the accel scale to 2G
        break; 
        }
        case ACCEL_RANGE_4G: {
        accelRangeRegValue = UB2_ACCEL_CONFIG_FS_SEL_4G;
        accelScale = G * 4.0f/accRawScaling; // setting the accel scale to 4G
        break;
        }
        case ACCEL_RANGE_8G: {
        accelRangeRegValue = UB2_ACCEL_CONFIG_FS_SEL_8G;
        accelScale = G * 8.0f/accRawScaling; // setting the accel scale to 8G
        break;
        }
        case ACCEL_RANGE_16G: {
        accelRangeRegValue = UB2_ACCEL_CONFIG_FS_SEL_16G;
        accelScale = G * 16.0f/accRawScaling; // setting the accel scale to 16G
        break;
        }
    }
    uint8_t dlpfRegValue = 0x00;
    switch(bandwidth) {
        case ACCEL_DLPF_BANDWIDTH_1209HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_1209HZ; break;
        case ACCEL_DLPF_BANDWIDTH_246HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_246HZ; break;
        case ACCEL_DLPF_BANDWIDTH_111HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_111HZ; break;
        case ACCEL_DLPF_BANDWIDTH_50HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_50HZ; break;
        case ACCEL_DLPF_BANDWIDTH_24HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_24HZ; break;
        case ACCEL_DLPF_BANDWIDTH_12HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_12HZ; break;
        case ACCEL_DLPF_BANDWIDTH_6HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_6HZ; break;
        case ACCEL_DLPF_BANDWIDTH_473HZ: dlpfRegValue = UB2_ACCEL_CONFIG_DLPFCFG_473HZ; break;
    }
    if (WriteReg(devAddr, UB2_ACCEL_CONFIG, accelRangeRegValue | dlpfRegValue) == false) {
        return -1;
    }
    _accelScale = accelScale;
    _accelRange = range;
    _accelBandwidth = bandwidth;
    return 1;
}

int IMU_ICM::enableAccelGyro() {
	if (changeUserBank(USER_BANK_0) < 0 || WriteReg(devAddr, UB0_PWR_MGMNT_2, UB0_PWR_MGMNT_2_SEN_ENABLE) == false) {
        return -1;
    }
    return 1;
}

/* gets the WHO_AM_I register value, expected to be 0xEA */
int IMU_ICM::whoAmI() {
	if (changeUserBank(USER_BANK_0) < 0) {
  	return -1;
  }
  // read the WHO AM I register
  if (readBytes(devAddr, UB0_WHO_AM_I, 1, _buffer) < 0) {
    return -1;
  }
  //printf("WhoAmI? %x\n",_buffer[0]);
  // return the register value
  return _buffer[0];
}

int IMU_ICM::whoAmIMag() {
	if (readMagRegisters(MAG_WHO_AM_I, 2, _buffer) < 0) {
    return -1;
  }
  return (_buffer[0] << 8) + _buffer[1];
}

int IMU_ICM::selectAutoClockSource() {
	if (changeUserBank(USER_BANK_0) < 0 || WriteReg(devAddr, UB0_PWR_MGMNT_1, UB0_PWR_MGMNT_1_CLOCK_SEL_AUTO) == false) {
    return -1;
  }
  return 1;
}

int IMU_ICM::resetMag() {
	if (writeMagRegister(MAG_CNTL3, MAG_CNTL3_RESET) < 0) {
		return -1;
        printf("Error\n");
	}
	return 1;
}

int IMU_ICM::reset() {
	if (changeUserBank(USER_BANK_0) < 0) {
    return -1;
  }
  if (WriteReg(devAddr, UB0_PWR_MGMNT_1, UB0_PWR_MGMNT_1_DEV_RESET) == false) {
  	return -2;
  }
  return 1;
}

int IMU_ICM::powerDownMag() {
	if (writeMagRegister(MAG_CNTL2, MAG_CNTL2_POWER_DOWN) < 0) {
		return -1;
	}
	return 1;
}

int IMU_ICM::enableI2cMaster() {
	if (changeUserBank(USER_BANK_0) < 0) {
        return -1;
    }
    if (WriteReg(devAddr, UB0_USER_CTRL, UB0_USER_CTRL_I2C_MST_EN) == false) {
        return -2;
    }
    if (changeUserBank(USER_BANK_3) < 0) {
        return -3;
    }
    if(WriteReg(devAddr, UB3_I2C_MST_CTRL, UB3_I2C_MST_CTRL_CLK_400KHZ) == false){
        return -4;
    }
    return 1;
}

int IMU_ICM::changeUserBank(UserBank userBank) {
	return changeUserBank(userBank, false);
}

int IMU_ICM::changeUserBank(UserBank userBank, bool force) {
	if (!force && userBank == _currentUserBank) {
		return 2; // No need to change
	}
	uint8_t userBankRegValue = 0x00;
	switch(userBank) {
    case USER_BANK_0: {
    	userBankRegValue = REG_BANK_SEL_USER_BANK_0;
  		break;
    }
    case USER_BANK_1: {
    	userBankRegValue = REG_BANK_SEL_USER_BANK_1;
  		break;
    }
    case USER_BANK_2: {
    	userBankRegValue = REG_BANK_SEL_USER_BANK_2;
  		break;
    }
    case USER_BANK_3: {
    	userBankRegValue = REG_BANK_SEL_USER_BANK_3;
  		break;
    }
  }
  if (WriteReg(devAddr, REG_BANK_SEL, userBankRegValue) == false) {
  	return -1;
  }
  _currentUserBank = userBank;
  return 1;
}

int IMU_ICM::readMagRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest) {
	if (changeUserBank(USER_BANK_3) < 0) {
  	    return -1;
    }
	if (WriteReg(devAddr, UB3_I2C_SLV0_ADDR, MAG_AK09916_I2C_ADDR | UB3_I2C_SLV0_ADDR_READ_FLAG) == false) {
        return -2;
    }
    // set the register to the desired magnetometer sub address
	if (WriteReg(devAddr, UB3_I2C_SLV0_REG, subAddress) == false) {
        return -3;
    }
    // enable I2C and request the bytes
	if (WriteReg(devAddr, UB3_I2C_SLV0_CTRL, UB3_I2C_SLV0_CTRL_EN | count) == false) {
        return -4;
     }
	usleep(1000); // takes some time for these registers to fill
    // read the bytes off the ICM-20948 EXT_SLV_SENS_DATA registers
    if (changeUserBank(USER_BANK_0) < 0) {
        return -5;
    }
	_status = readBytes(devAddr, UB0_EXT_SLV_SENS_DATA_00, count, dest); 
    return _status;
}

int IMU_ICM::writeMagRegister(uint8_t subAddress, uint8_t data) {
	if (changeUserBank(USER_BANK_3) < 0) {
  	    return -1;
    }
	if (WriteReg(devAddr, UB3_I2C_SLV0_ADDR, MAG_AK09916_I2C_ADDR) == false) {
        return -2;
    }
    // set the register to the desired magnetometer sub address 
	if (WriteReg(devAddr, UB3_I2C_SLV0_REG, subAddress) == false) {
        return -3;
    }
    // store the data for write
	if (WriteReg(devAddr, UB3_I2C_SLV0_DO, data) == false) {
        return -4;
    }
    // enable I2C and send 1 byte
	if (WriteReg(devAddr, UB3_I2C_SLV0_CTRL, UB3_I2C_SLV0_CTRL_EN | (uint8_t)1) == false) {
        return -5;
    }
	// read the register and confirm
	if (readBytes(devAddr, subAddress, 21, _buffer) == false) {
        return -6;
    }
	if(_buffer[0] != data) {
        /* printf("Buffer: ");
        for (int i=0; i<21; i++){
            printf("%d ", _buffer[i]);
        }
        printf("\n");
        printf("\nbuffer: %x %x %x\n", _buffer[0], _buffer[1], data); */
  	    return -7;
    }
    return 1;
}

bool IMU_ICM::WriteReg(uint32_t devAddr, uint8_t RegIndex, uint8_t RegValue){
    int ack = 0;
    ack |= I2C_start(m_ControllerAddr, devAddr, 0);
    ack |= I2C_write(m_ControllerAddr, RegIndex, 0);
    ack |= I2C_write(m_ControllerAddr, RegValue, 1);
    if (ack != 0) {
        #ifdef  I2C_DEBUG
    	    printf("\t[IMU_ICM] DeviceAddress: %lx, RegIndex: %hhx, RegValue: %hhx\n", (long unsigned int)devAddr, RegIndex, RegValue);
        #endif
    }
    else
    {   
        //printf("\tSuccess\n");
    }
    return ((ack == I2C_ACK)? true: false);
}

bool IMU_ICM::readBytes(uint32_t devAddr, uint8_t RegIndex, uint16_t Len, uint8_t *pRegValue){
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

bool IMU_ICM::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitIndex, bool bSet){
	return writeBits(devAddr, RegIndex, BitIndex, 1, bSet?0x01:0x00);
}

bool IMU_ICM::writeBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t Value){
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

bool IMU_ICM::readBits(uint32_t devAddr, uint8_t RegIndex, uint8_t BitEndIndex, uint8_t BitNum, uint8_t *Buffer){
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