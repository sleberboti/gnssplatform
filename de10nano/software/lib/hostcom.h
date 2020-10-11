#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */ 
#include <errno.h>   /* ERROR Number Definitions           */

#include <queue> 
#include <string> 
#include "def.h"

class HostCom {
private:
    int fd;
    struct termios SerialPortSettings;

    
    char read_buffer[32];   // Buffer to store the data received 
    char write_buffer[32];             
    int  bytes_read = 0;    // Number of bytes read by the read() system call 
    int bytes_written = 0;  // Number of bytes written
    
    bool equalHost;
    bool equalImucv5;
    bool equalImuicm;
    bool equalMm;
    bool equalMs;
    bool equalUblox;
    bool equalFreq;
    bool equalDur;

    // so ugly :(, SocEds gcc complier does not recognize const char abcd[5]="abcd"
    const char hostText[7] = {'|','H','o','s','t','|','\0'};    
    const char imu_cv5Text[8] = {'i','m','u','_','c','v','5','\0'};
    const char imu_icmText[8] = {'i','m','u','_','i','c','m','\0'};
    const char mmText[3] = {'m','m','\0'};
    const char msText[3] = {'m','s','\0'};
    const char ubloxText[6] = {'u','b','l','o','x','\0'};
    const char freqText[5] = {'f','r','e','q','\0'};
    const char durText[4] = {'d', 'u', 'r', '\0'};
    
protected:

public:
	HostCom();
	virtual ~HostCom();
    bool initialize();
    bool setInterfaceAttributes();
    void listener(std::queue <std::string> &myqueue, bool hostcommand);
    void writer(sensorPacket mySensor);
};
