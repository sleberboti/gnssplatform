#include "ublox.h"
#include "i2c_opencores.h"
#include <iostream>

//#define I2C_DEBUG
//#define UBLOX_DEBUG

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
    //printf("calcChecksum:\n");
    /* for (int i=0; i< bufferSize; i++){
        //printf("%d ", messageBuffer[i]);
        //printf("%x ", messageBuffer[i]);
    } */
    uint8_t CK_A = 0;
    uint8_t CK_B = 0;
    //printf("CalcChesum started\n");
    for(int i=0; i<bufferSize-2; i++){ // was buffersize-2
        CK_A = CK_A + messageBuffer[i];
        CK_B = CK_B + CK_A;
    }
    //printf("\nCalced Cs: %x %x\n", CK_A, CK_B);

    if(CK_A == messageBuffer[bufferSize-2] and CK_B == messageBuffer[bufferSize-1]){
        //printf("CS: TRUEEE\n");
        return true;        
    }
    else{
        //printf("CS: false\n");
        return false;
    }
    //printf("CalcChesum ended\n");
}

//sensorPacket
void UBLOX::reportMeasx(){
    printf("\nRXM Measx packet reported\n");

    packetNow.readMeasx = true;

    packetNow.sensorname = "ublox_measx";
    packetNow.version = measx_validPacket.version;
    packetNow.gpsTOW = measx_validPacket.gpsTOW;       //[ms]
    packetNow.gloTOW = measx_validPacket.gloTOW;       //[ms]
    packetNow.bdsTOW = measx_validPacket.bdsTOW;       //[ms]
    packetNow.qzssTOW = measx_validPacket.qzssTOW;      //[ms]
    packetNow.gpsTOWacc = measx_validPacket.gpsTOWacc;    //[ms]
    packetNow.gloTOWacc = measx_validPacket.gloTOWacc;    //[ms]
    packetNow.bdsTOWacc = measx_validPacket.bdsTOWacc;    //[ms]
    packetNow.qzssTOWacc = measx_validPacket.qzssTOWacc;   //[ms]
    packetNow.measx_numSV = measx_validPacket.measx_numSV;
    packetNow.measx_flags = measx_validPacket.measx_flags;

    memcpy(packetNow.measx_repeated, measx_validPacket.measx_repeated, sizeof(packetNow.measx_repeated));

    /* for(int i=0; i<measx_validPacket.measx_numSV; i++){
        printf("%i, %i\n", i, packetNow.measx_repeated[i].measx_gnssId);
    }  */ 

    //return packetNow;
}

//sensorPacket
void UBLOX::reportSfrbx(){

    packetNow.readSfrbx = true;

    printf("\nRXM Sfrbx packet reported\n");

    packetNow.sensorname = "ublox_sfrbx";
    packetNow.gnssId = sfrbx_validPacket.gnssId;
    packetNow.svId = sfrbx_validPacket.svId;
    packetNow.freqId = sfrbx_validPacket.freqId;
    packetNow.numWords = sfrbx_validPacket.numWords;    // up to 10
    packetNow.chn = sfrbx_validPacket.chn;    
    packetNow.sfrbx_version = sfrbx_validPacket.sfrbx_version;

    //printf("\nSfrbx memcpy\n");
    memcpy(packetNow.dwrd, sfrbx_validPacket.drwd, sizeof(packetNow.dwrd));

    //return packetNow;
}

//sensorPacket
void UBLOX::reportPVT(){

    printf("\nPVT packet reported\n");

    packetNow.readPvt = true;

    packetNow.sensorname = "ublox_pvt";
    packetNow.iTOW = validPacket.iTOW;    
    packetNow.year = validPacket.year;
    packetNow.month = validPacket.month;
    packetNow.day = validPacket.day;
    packetNow.hour = validPacket.hour;
    packetNow.min = validPacket.min;
    packetNow.sec = validPacket.sec;
    packetNow.valid = validPacket.valid;
    packetNow.tAcc = validPacket.valid;
    packetNow.nano = validPacket.valid;
    packetNow.fixType = validPacket.valid;
    packetNow.flags = validPacket.flags;
    packetNow.flags2 = validPacket.flags2;
    packetNow.numSV = validPacket.numSV;
    packetNow.lat = (double)validPacket.lat*1e-7;
    packetNow.lon = (double)validPacket.lon*1e-7;
    packetNow.height = (double)validPacket.height;
    packetNow.hMSL = (double)validPacket.hMSL;
    packetNow.hAcc = (double)validPacket.hAcc;
    packetNow.vAcc = (double)validPacket.vAcc;
    packetNow.velN = (double)validPacket.velN;
    packetNow.velE = (double)validPacket.velE;
    packetNow.velD = (double)validPacket.velD;
    packetNow.gSpeed = (double)validPacket.gSpeed;
    packetNow.headMot = (double)validPacket.headMot*1e-5;
    packetNow.sAcc = (double)validPacket.sAcc;
    packetNow.headAcc = (double)validPacket.headAcc*1e-5;
    packetNow.pDOP = (float)validPacket.pDOP*1e-2;
    packetNow.headVeh = (double)validPacket.headVeh*1e-5;
    packetNow.magDec = (float)validPacket.magDec*1e-2;
    packetNow.magAcc = (float)validPacket.magAcc*1e-2;

    #ifdef UBLOX_DEBUG
    printf("msg_class:%d\n",validPacket.msg_class);
    printf("id:%d\n",validPacket.msg_id);
    printf("msg_lenght:%d\n",validPacket.msg_length);
    printf("iTOW:%d\n",validPacket.iTOW);    
    printf("year:%d\n",validPacket.year);
    printf("month:%d\n",validPacket.month);
    printf("day:%d\n",validPacket.day);
    printf("hour:%d\n",validPacket.hour);
    printf("min:%d\n",validPacket.min);                    
    printf("sec:%d\n",validPacket.sec);
    printf("valid:%d\n",validPacket.valid);
    printf("validDate:%d\n",(validPacket.valid & 0x01));
    printf("validTime:%d\n",(validPacket.valid & 0x02));  
    printf("isTimeFullyResolved:%d\n",(validPacket.valid & 0x04));
    printf("isMagneticDeclinationValid:%d\n",(validPacket.valid & 0x08));
    printf("tAcc:%d\n",validPacket.tAcc); 
    printf("nano:%d\n",validPacket.nano); 
    printf("fixType:%d\n",(FixType)validPacket.fixType); 
    printf("flags1:%d\n",validPacket.flags);
    printf("PowerSaveMode:%d\n",(PowerSaveMode)((validPacket.flags >> 2) & 0x07));
    printf("CarrierPhaseStatus:%d\n",(CarrierPhaseStatus)((validPacket.flags >> 6) & 0x03));
    printf("isGnssFixOk:%d\n",(validPacket.flags & 0x01));
    printf("isDiffCorrApplied:%d\n",(validPacket.flags & 0x02));
    printf("isHeadingValid:%d\n",(validPacket.flags & 0x20));
    printf("isConfirmedDate:%d\n",(validPacket.flags & 0x40));
    printf("isConfirmedTime:%d\n",(validPacket.flags & 0x80));
    printf("flags2:%d\n",validPacket.flags2);
    printf("isTimeDateConfirmationAvail:%d\n",(validPacket.flags2 & 0x20));
    printf("numSV:%d\n",validPacket.numSV); 
    printf("lat:%f\n",(double)validPacket.lat*1e-7);
    printf("lon:%f\n",(double)validPacket.lon*1e-7);
    printf("height:%f\n",(double)validPacket.height); 
    printf("hMSL:%f\n",(double)validPacket.hMSL); 
    printf("hAcc:%f\n",(double)validPacket.hAcc); 
    printf("vAcc:%f\n",(double)validPacket.vAcc); 
    printf("velN:%f\n",(double)validPacket.velN); 
    printf("velE:%f\n",(double)validPacket.velE); 
    printf("velD:%f\n",(double)validPacket.velD); 
    printf("gSpeed:%f\n",(double)validPacket.gSpeed); 
    printf("headMot:%f\n",(double)validPacket.headMot*1e-5); 
    printf("sAcc:%f\n",(double)validPacket.sAcc); 
    printf("headAcc:%f\n",(double)validPacket.headAcc*1e-5); 
    printf("pDop:%f\n",(float)validPacket.pDOP*1e-2);
    printf("headVeh:%f\n",(double)validPacket.headVeh*1e-5);
    printf("magDec:%f\n",(float)validPacket.magDec*1e-2);
    printf("magAcc:%f\n",(float)validPacket.magAcc*1e-2);
    #endif

    //return packetNow;
}


sensorPacket UBLOX::getPvtBuffer(){
    // GETTING Pvt from Buffer
    int bufferSize = sizeof(buffer) / sizeof(buffer[0]);
    
    checksum = false;
    while(!checksum){
        //printf("Buffer: ");
        readBytes(devAddr, 0xff, bufferSize, buffer); 
        for (int i=0; i<bufferSize; i++){
            if (buffer[i+2]==ubxNavPvt_msgClass && buffer[i+3]==ubxNavPvt_msgId){
                    printf("\nPVT packet found\n");
                    for (int j=0; j<ubxNavPvt_msgLen; j++){
                        byte = buffer[i+2+j];
                        //printf("%d ", byte);
                        *((uint8_t *) &tempPacket+j) = byte;
                        ubxNavPvt_buffer[j] = byte;
                    }
                    ubxNavPvt_buffer[ubxNavPvt_msgLen]= buffer[i+2+ubxNavPvt_msgLen];
                    ubxNavPvt_buffer[ubxNavPvt_msgLen+1]= buffer[i+2+ubxNavPvt_msgLen+1];

                    //printf("\n Cs data: %x %x\n", ubxNavPvt_buffer[ubxNavPvt_msgLen], ubxNavPvt_buffer[ubxNavPvt_msgLen+1]); //checksums
                    checksum = calcChecksum(ubxNavPvt_buffer, ubxNavPvt_msgLen+2);
                    if (checksum){
                        validPacket = tempPacket;
                        reportPVT();
                    }
                    //printf("Cs result: %d\n\n", cs);
            }
        }
    }
    return packetNow;
}

sensorPacket UBLOX::getMeasxBuffer(){
    // GETTING Measx from Buffer
    int bufferSize = sizeof(buffer) / sizeof(buffer[0]);
    
    checksum = false;
    while(!checksum){
        //printf("Buffer: ");
        readBytes(devAddr, 0xff, bufferSize, buffer); 
        for (int i=0; i<bufferSize; i++){
            if (buffer[i]==ubxHeader[0] && buffer[i+1]==ubxHeader[1]){
                int ubxRxmMeasx_baseLength = 44+4;
                if (i>=bufferSize-ubxRxmMeasx_baseLength) break;
                if (buffer[i+2]==ubxRxmMeasx_msgClass && buffer[i+3]==ubxRxmMeasx_msgId){
                    printf("\nRAW Measx packet found\n");
                    uint8_t ubxRxmMeasx_buffer[ubxRxmMeasx_msgLen];
                    
                    int ubxRxmMeasx_dataLength = 0;
                    int ubxRxmMeasx_baseLength = 44+4;

                    ubxRxmMeasx_dataLength = ubxRxmMeasx_baseLength;

                    for (int j=0; j<ubxRxmMeasx_baseLength; j++){
                        //if(j==4) printf("\n");
                        byte = buffer[i+2+j];
                        //printf("%x ", byte);
                        *((uint8_t *) &measx_tempPacket+j) = byte;
                        ubxRxmMeasx_buffer[j] = byte;
                    }

                    //printf("\n Measx NumSV: %d\n",measx_tempPacket.measx_numSV);

                    if (i>=bufferSize-ubxRxmMeasx_baseLength-(measx_tempPacket.measx_numSV)*24) break;

                    for (int k=0; k<(measx_tempPacket.measx_numSV)*24; k++){
                        //if(k%4==0) printf("\n\t");
                        byte = buffer[i+2+ubxRxmMeasx_baseLength+k];
                        *((uint8_t *) &measx_tempPacket+ubxRxmMeasx_baseLength+k) = byte;
                        //*((uint8_t *) &measx_repeated+k) = byte;
                        //printf("%x ", byte);
                        ubxRxmMeasx_buffer[ubxRxmMeasx_baseLength+k] = byte;       
                    }
                    ubxRxmMeasx_dataLength = ubxRxmMeasx_baseLength + (measx_tempPacket.measx_numSV)*24;

                    ubxRxmMeasx_buffer[ubxRxmMeasx_dataLength]= buffer[i+2+ubxRxmMeasx_dataLength];
                    ubxRxmMeasx_buffer[ubxRxmMeasx_dataLength+1]= buffer[i+2+ubxRxmMeasx_dataLength+1];
                    ubxRxmMeasx_dataLength += 2; // added checksum

                    checksum = calcChecksum(ubxRxmMeasx_buffer, ubxRxmMeasx_dataLength);

                    if (checksum){
                        measx_validPacket = measx_tempPacket;
                        //return reportMeasx();
                        break;
                    }
                    else{
                        return packetNow;
                    }

                }
            }
        }
    } 
    return packetNow; // was commented
}

sensorPacket UBLOX::getUbloxBuffer(){
    int bufferSize = sizeof(buffer) / sizeof(buffer[0]);    
    //printf("wtf? %d", bufferSize);
    
    packetNow.readPvt   = false;
    packetNow.readMeasx = false;
    packetNow.readSfrbx = false;

    checksum = false;

    readBytes(devAddr, 0xff, bufferSize, buffer); 
    for (int i=0; i<bufferSize-500; i++){
        //printf("%x ", buffer[i]);
        //printf("%d ", i);
        if (buffer[i]==ubxHeader[0] && buffer[i+1]==ubxHeader[1]){
            if (buffer[i+2]==ubxNavPvt_msgClass && buffer[i+3]==ubxNavPvt_msgId){
                //printf("\nPVT packet found\n");
                for (int j=0; j<ubxNavPvt_msgLen; j++){
                    byte = buffer[i+2+j];
                    //printf("%d ", byte);
                    *((uint8_t *) &tempPacket+j) = byte;
                    ubxNavPvt_buffer[j] = byte;
                }
                ubxNavPvt_buffer[ubxNavPvt_msgLen]= buffer[i+2+ubxNavPvt_msgLen];
                ubxNavPvt_buffer[ubxNavPvt_msgLen+1]= buffer[i+2+ubxNavPvt_msgLen+1];

                //printf("\n PVT NumSV: %d\n",tempPacket.numSV);

                //printf("\n Cs data: %x %x\n", ubxNavPvt_buffer[ubxNavPvt_msgLen], ubxNavPvt_buffer[ubxNavPvt_msgLen+1]); //checksums
                checksum = calcChecksum(ubxNavPvt_buffer, ubxNavPvt_msgLen+2);
                if (checksum){
                    validPacket = tempPacket;
                    reportPVT();
                }
                //printf("Cs result: %d\n\n", cs);                    

            }

            if (buffer[i+2]==ubxRxmSfrbx_msgClass && buffer[i+3]==ubxRxmSfrbx_msgId){
                //printf("\nRXM Sfrbx packet found\n");

                //ubxRxmSfrbx_msgLen = 8;                    
                uint8_t ubxRxmSfrbx_buffer[ubxRxmSfrbx_msgLen];

                int ubxRxmSfrbx_dataLength = 0;
                int ubxRxmSfrbx_baseLength = 8+4;

                ubxRxmSfrbx_dataLength = ubxRxmSfrbx_baseLength;
                for (int j=0; j<ubxRxmSfrbx_baseLength; j++){
                    //if(j==4) printf("\n");
                    byte = buffer[i+2+j];
                    //printf("%x ", byte);
                    *((uint8_t *) &sfrbx_tempPacket+j) = byte;
                    ubxRxmSfrbx_buffer[j] = byte;
                }
                
                //printf("GnssID: %d Num words: %d \n", sfrbx_tempPacket.gnssId, sfrbx_tempPacket.numWords);

                for (int k=0; k<(sfrbx_tempPacket.numWords)*4; k++){
                    //if(k%4==0) printf("\n\t");
                    byte = buffer[i+2+ubxRxmSfrbx_baseLength+k];
                    *((uint8_t *) &sfrbx_tempPacket+ubxRxmSfrbx_baseLength+k) = byte;
                    //printf("%x ", byte);
                    ubxRxmSfrbx_buffer[ubxRxmSfrbx_baseLength+k] = byte;       
                }
                ubxRxmSfrbx_dataLength = ubxRxmSfrbx_baseLength + (sfrbx_tempPacket.numWords)*4;

                ubxRxmSfrbx_buffer[ubxRxmSfrbx_dataLength]= buffer[i+2+ubxRxmSfrbx_dataLength];
                ubxRxmSfrbx_buffer[ubxRxmSfrbx_dataLength+1]= buffer[i+2+ubxRxmSfrbx_dataLength+1];
                ubxRxmSfrbx_dataLength += 2; // added checksum
                //printf("Checksums: %x %x\n", ubxRxmSfrbx_buffer[ubxRxmSfrbx_dataLength], ubxRxmSfrbx_buffer[ubxRxmSfrbx_dataLength+1]);

                /* for(int g=0; g<sfrbx_tempPacket.numWords; g++){
                    printf("\nDrwd[%d]: %x\n", g, sfrbx_tempPacket.drwd[g]);
                } */
                
                checksum = calcChecksum(ubxRxmSfrbx_buffer, ubxRxmSfrbx_dataLength);

                if (checksum){
                    sfrbx_validPacket = sfrbx_tempPacket;
                    reportSfrbx();
                }
            }

            if (buffer[i+2]==ubxRxmMeasx_msgClass && buffer[i+3]==ubxRxmMeasx_msgId){
                //printf("\nRAW Measx packet found\n");
                uint8_t ubxRxmMeasx_buffer[ubxRxmMeasx_msgLen];
                
                int ubxRxmMeasx_dataLength = 0;
                int ubxRxmMeasx_baseLength = 44+4;

                ubxRxmMeasx_dataLength = ubxRxmMeasx_baseLength;

                for (int j=0; j<ubxRxmMeasx_baseLength; j++){
                    //if(j==4) printf("\n");
                    byte = buffer[i+2+j];
                    //printf("%x ", byte);
                    *((uint8_t *) &measx_tempPacket+j) = byte;
                    ubxRxmMeasx_buffer[j] = byte;
                }

                //printf("\n Measx NumSV: %d\n",measx_tempPacket.measx_numSV);

                for (int k=0; k<(measx_tempPacket.measx_numSV)*24; k++){
                    //if(k%4==0) printf("\n\t");
                    byte = buffer[i+2+ubxRxmMeasx_baseLength+k];
                    *((uint8_t *) &measx_tempPacket+ubxRxmMeasx_baseLength+k) = byte;
                    //*((uint8_t *) &measx_repeated+k) = byte;
                    //printf("%x ", byte);
                    ubxRxmMeasx_buffer[ubxRxmMeasx_baseLength+k] = byte;       
                }
                ubxRxmMeasx_dataLength = ubxRxmMeasx_baseLength + (measx_tempPacket.measx_numSV)*24;

                ubxRxmMeasx_buffer[ubxRxmMeasx_dataLength]= buffer[i+2+ubxRxmMeasx_dataLength];
                ubxRxmMeasx_buffer[ubxRxmMeasx_dataLength+1]= buffer[i+2+ubxRxmMeasx_dataLength+1];
                ubxRxmMeasx_dataLength += 2; // added checksum

                checksum = calcChecksum(ubxRxmMeasx_buffer, ubxRxmMeasx_dataLength);

                if (checksum){
                    measx_validPacket = measx_tempPacket;
                    reportMeasx();
                }
            }
        }
    }

    return packetNow;
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
    	    printf("\t[UBLOX] DeviceAddress: %lx, RegIndex: %hhx\n", (long unsigned int)devAddr, RegIndex);
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