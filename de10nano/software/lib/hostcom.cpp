#include "hostcom.h"

#include <string>
#include <iostream>
#include <thread>


HostCom::HostCom()
{
    bool initSuccess = initialize();
    if (initSuccess == false){
        printf("Error during initializing HostCom\n");
    }
}

HostCom::~HostCom() {
}

void HostCom::writer(sensorPacket mySensor){

    printf("%s\n", mySensor.sensorname.c_str());

    if (mySensor.sensorname == "imu_cv5"){
        printf("|DE10|name:%s,acc:%f,%f,%f,gyro:%f,%f,%f,tow:%llu,|\n",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.acc[0],
                                                        mySensor.acc[1],
                                                        mySensor.acc[2],
                                                        mySensor.gyro[0],
                                                        mySensor.gyro[1],
                                                        mySensor.gyro[2],
                                                        mySensor.tow);
    }
    if (mySensor.sensorname == "imu_icm"){
        printf("|DE10|name:%s,"
               "accel_xout:%f,"
               "accel_yout:%f,"
               "accel_zout:%f,"
               "gyro_xout:%f,"
               "gyro_yout:%f,"
               "gyro_zout:%f,"
               "mm_xout:%f,"
               "mm_yout:%f,"
               "mm_zout:%f,"
               "temp_out:%f,"
               "tow:%llu,|\n",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.accel_xout,
                                                        mySensor.accel_yout,
                                                        mySensor.accel_zout,
                                                        mySensor.gyro_xout,
                                                        mySensor.gyro_yout,
                                                        mySensor.gyro_zout,
                                                        mySensor.mm_xout,
                                                        mySensor.mm_yout,
                                                        mySensor.mm_zout,
                                                        mySensor.temp_out,                                                        
                                                        mySensor.icm_itow);
    }
    if (mySensor.sensorname == "mm"){
        printf("|DE10|name:%s,x:%d,y:%d,z:%d,tow:%llu,|\n",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.mm[0],
                                                        mySensor.mm[1],
                                                        mySensor.mm[2],
                                                        mySensor.mm_itow);
                                                        
    }
    if (mySensor.sensorname == "ms"){
        printf("|DE10|name:%s,temp:%f,pressure:%d,tow:%llu,|\n",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.temperature,
                                                        mySensor.pressure,                                                        
                                                        mySensor.ms5611_itow);
                                                        
    }
    if (mySensor.sensorname == "ublox_measx"){
        if (mySensor.sensorname == "ublox_measx"){
        printf("|DE10|name:%s,"
                    "gpsTOW:%d,"
                    "gloTOW:%d,"
                    "bdsTOW:%d,"
                    "gzssTOW:%d,"
                    "gpsTOWacc:%d,"
                    "gloTOWacc:%d,"
                    "bdsTOWacc:%d,"
                    "qzssTOWacc:%d,"
                    "measx_numSV:%d,"
                    "measx_flags:%d,",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.gpsTOW,   
                                                        mySensor.gloTOW,
                                                        mySensor.bdsTOW,
                                                        mySensor.qzssTOW,
                                                        mySensor.gpsTOWacc,
                                                        mySensor.gloTOWacc,
                                                        mySensor.bdsTOWacc,
                                                        mySensor.qzssTOWacc,
                                                        mySensor.measx_numSV,
                                                        mySensor.measx_flags);
        }
        for(int i=0; i<mySensor.measx_numSV; i++){
            printf("gnssId:%d,"
                    "svId:%d,"
                    "cNo:%d,"
                    "mpathIndic:%d,"
                    "dopplerMS:%d,"
                    "measx_dopplerHz:%d,"
                    "measx_wholeChips:%d,"
                    "fracChips:%d,"
                    "codePhase:%d,"
                    "intCodePhase:%d,"
                    "pseuRangeRMSErr:%d,",
                                                        mySensor.measx_repeated[i].measx_gnssId,
                                                        mySensor.measx_repeated[i].measx_svId,
                                                        mySensor.measx_repeated[i].measx_cNo,
                                                        mySensor.measx_repeated[i].measx_mpathIndic,
                                                        mySensor.measx_repeated[i].measx_dopplerMS,
                                                        mySensor.measx_repeated[i].measx_dopplerHz,
                                                        mySensor.measx_repeated[i].measx_wholeChips,
                                                        mySensor.measx_repeated[i].measx_fracChips,
                                                        mySensor.measx_repeated[i].measx_codePhase,
                                                        mySensor.measx_repeated[i].measx_intCodePhase,
                                                        mySensor.measx_repeated[i].measx_pseuRangeRMSErr
                                                        );
        }
        printf("|\n");
    }

    if (mySensor.sensorname == "ublox_sfrbx"){
        if (mySensor.sensorname == "ublox_sfrbx"){
        printf("|DE10|name:%s,"
                    "gnssId:%d,"
                    "svId:%d,"
                    "freqId:%d,"
                    "numWords:%d,"
                    "chn:%d,",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.gnssId,   
                                                        mySensor.svId,
                                                        mySensor.freqId,
                                                        mySensor.numWords,
                                                        mySensor.chn);
        }
        for(int i=0; i<mySensor.numWords; i++){
            printf("dwrd:%x,",
                                                        mySensor.dwrd[i]);
        }
        
        printf("|\n");
    }

    if (mySensor.sensorname == "ublox_pvt"){
        if (mySensor.sensorname == "ublox_pvt"){
        printf("|DE10|name:%s,"
                    "iTow:%d,"
                    "year:%d,"
                    "month:%d,"
                    "day:%d,"
                    "hour:%d,"
                    "min:%d,"
                    "sec:%d,"
                    "valid:%d,"
                    "tAcc:%d,"
                    "nano:%d,"
                    "fixType:%d,"
                    "flags:%d,"
                    "flags2:%d,"
                    "numSV:%d,"
                    "lat:%f,"
                    "lon:%f,"
                    "height:%f,"
                    "hMSL:%f,"
                    "hAcc:%f,"
                    "vAcc:%f,"
                    "velN:%f,"
                    "velE:%f,"
                    "velD:%f,"
                    "gSpeed:%f,"
                    "headMot:%f,"
                    "sAcc:%f,"
                    "headAcc:%f,"
                    "pDOP:%f,"
                    "headVeh:%f,"
                    "magDec:%f,"
                    "magAcc:%f,|\n",
                                                        mySensor.sensorname.c_str(),
                                                        mySensor.iTOW,   
                                                        mySensor.year,
                                                        mySensor.month,
                                                        mySensor.day,
                                                        mySensor.hour,
                                                        mySensor.min,
                                                        mySensor.sec,
                                                        mySensor.valid,
                                                        mySensor.tAcc,
                                                        mySensor.nano,
                                                        mySensor.fixType,
                                                        mySensor.flags,
                                                        mySensor.flags2,
                                                        mySensor.numSV,
                                                        mySensor.lat,
                                                        mySensor.lon,
                                                        mySensor.height,
                                                        mySensor.hMSL,
                                                        mySensor.hAcc,
                                                        mySensor.vAcc,
                                                        mySensor.velN,
                                                        mySensor.velE,
                                                        mySensor.velD,
                                                        mySensor.gSpeed,
                                                        mySensor.headMot,
                                                        mySensor.sAcc,
                                                        mySensor.headAcc,
                                                        mySensor.pDOP,
                                                        mySensor.headVeh,
                                                        mySensor.magDec,
                                                        mySensor.magAcc);
        }
    }
}


void HostCom::listener(std::queue <std::string> &myqueue, bool hostcommand){

    printf("HostCom::listener\n");

    if (hostcommand){
        while(true){
            char clear_buffer[32] = "";
            bytes_read = read(fd,read_buffer,32); // Read the data
            
            equalHost = true;
            equalImucv5 = true;
            equalImuicm = true;
            equalMm = true;
            equalUblox = true;
            equalMs = true;
            
            if(bytes_read>0){
                char read_data[32];
                memcpy(read_data,read_buffer,32);
                memcpy(read_buffer, clear_buffer, 32);
                printf("Bytes read %d %s\n",bytes_read, read_buffer);            
                for (int i=1; i<6; i++){
                    if (read_data[i]==hostText[i]){                    
                        // It means that the read buffer starts with |Host| sequence
                    }
                    else{    
                        equalHost=false;
                    }
                }
                if(equalHost){
                    char* sub_buffer = read_data + 6;                
                    //printf("%c%c%c%c\n",sub_buffer[0],sub_buffer[1],sub_buffer[2],sub_buffer[3]);

                    for (int i=0; i<4; i++){
                        if (sub_buffer[i]==freqText[i]){      
                            equalFreq = true;
                        }
                        else{
                            equalFreq = false;
                        }
                    }

                    for (int i=0; i<3; i++){
                        if (sub_buffer[i]==durText[i]){      
                            equalDur = true;
                        }
                        else{
                            equalDur = false;
                        }
                    }

                    for (int i=0; i<7; i++){
                        if (sub_buffer[i]==imu_cv5Text[i]){                    
                        // It means that the read buffer follows with imu_cv5 sequence
                        }
                        else{    
                            equalImucv5=false;
                        }
                    }
                    for (int i=0; i<7; i++){
                        if (sub_buffer[i]==imu_icmText[i]){                    
                        // It means that the read buffer follows with imu_icm sequence
                        }
                        else{    
                            equalImuicm=false;
                            //printf("NotIMuICM\n");
                        }
                    }
                    for (int i=0; i<2; i++){
                        //printf("EqualMM: %c %c\n",sub_buffer[i],mmText[i]);
                        if (sub_buffer[i]==mmText[i]){                    
                        // It means that the read buffer follows with mm sequence
                        }
                        else{    
                            equalMm=false;
                        }
                    }
                    for (int i=0; i<2; i++){
                        //printf("EqualMM: %c %c\n",sub_buffer[i],mmText[i]);
                        if (sub_buffer[i]==msText[i]){                    
                        // It means that the read buffer follows with ms sequence
                        }
                        else{    
                            equalMs=false;
                        }
                    }
                    for (int i=0; i<5; i++){
                        //printf("EqualUblox: %c and %c\n",sub_buffer[i],ubloxText[i]);
                        if (sub_buffer[i]==ubloxText[i]){                    
                        // It means that the read buffer follows with ublox sequence
                        }
                        else{    
                            equalUblox=false;
                        }
                    }

                    if(equalFreq){
                        
                        int freq_end;
                        for(int i=0; i<10; i++){
                            if (sub_buffer[i]=='|'){
                                freq_end = i;
                                break;
                            }
                        }
                        char freq_str[10];
                        strncpy (freq_str, sub_buffer+5, freq_end+1);
                        freq_str[freq_end+1] = '\0';
                        printf("Freq: %s\n", freq_str);
                        myqueue.push(freq_str);
                    }

                    if(equalDur){
                        
                        int dur_end;
                        for(int i=0; i<10; i++){
                            if (sub_buffer[i]=='|'){
                                dur_end = i;
                                break;
                            }
                        }
                        char dur_str[10];
                        strncpy (dur_str, sub_buffer+4, dur_end+1);
                        dur_str[dur_end+1] = '\0';
                        printf("Duration: %s\n", dur_str);
                        myqueue.push(dur_str);
                    }

                    if(equalImucv5){
                        myqueue.push("imu_cv5");
                        printf("Host asks for imu_cv5\n");
                    }
                    if(equalImuicm){
                        myqueue.push("imu_icm");
                        printf("Host asks for imu_icm\n");
                    }
                    if(equalMm){
                        myqueue.push("mm");
                        printf("Host asks for mm\n");
                    }
                    if(equalUblox){
                        myqueue.push("ublox");
                        printf("Host asks for ublox\n");
                    }
                    if(equalMs){
                        myqueue.push("ms");
                        printf("Host asks for ms\n");
                    }
                }
            }
        }    
    }    
}

bool HostCom::setInterfaceAttributes(){
    bool bSuccess = true;

    //---------- Setting the Attributes of the serial port using termios structure ---------                
    tcgetattr(fd, &SerialPortSettings);                            // Get the current attributes of the Serial port
    // Setting the Baud rate
    cfsetispeed(&SerialPortSettings,B115200);                      // Set Read  Speed as 115200                      
    cfsetospeed(&SerialPortSettings,B115200);                      // Set Write Speed as 115200                  

    SerialPortSettings.c_cflag &= ~PARENB;                         // Disables the Parity Enable bit(PARENB),So No Parity   
    SerialPortSettings.c_cflag &= ~CSTOPB;                         // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
    SerialPortSettings.c_cflag &= ~CSIZE;                          // Clears the mask for setting the data size             
    SerialPortSettings.c_cflag |=  CS8;                            // Set the data bits = 8                                 
    SerialPortSettings.c_cflag &= ~CRTSCTS;                        // No Hardware flow Control                         
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines        
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable XON/XOFF flow control both i/p and o/p 
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Non Cannonical mode 
    //SerialPortSettings.c_oflag &= ~OPOST;//No Output Processing ---- messes up termial, so commented out
    // Setting Timeouts     
    SerialPortSettings.c_cc[VMIN] = 13; // Read at least 10 characters 
    SerialPortSettings.c_cc[VTIME] = 0; // Wait indefinetly

    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0){
        printf("Error while setting attributes \n"); // Set the attributes to the termios structure
    } 

    return bSuccess;
}

bool HostCom::initialize() {
    bool bSuccess = true;
    //------------------------------- Opening the Serial Port -------------------------------
    fd = open("/dev/ttyS0",O_RDWR | O_NOCTTY);    
    if(fd == -1){
        printf("Error while opening the device\n");
        bSuccess = false;
    }
    
    bSuccess = setInterfaceAttributes();      
    //printf("%s\n", bSuccess ? "true" : "false");
    
    return bSuccess;    
}