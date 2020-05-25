#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include <string.h>

#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define REGS_BASE (ALT_LWFPGASLVS_OFST)
#define REGS_SPAN (0x00200000)
void *virtual_base;
void *leds_addr;
void *switch_addr;

int peripherals()
{
	int fd;

	printf("ALT_STM_OFST Value: 0x%08x\n", ALT_STM_OFST);
	printf("ALT_LWFPGASLVS_OFST Value: 0x%08x\n", ALT_LWFPGASLVS_OFST);

	if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1) 
	{
		printf("ERROR: could not open \"/dev/mem\"...\n");
		return -1;
	}

	virtual_base = mmap(NULL, REGS_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, REGS_BASE);

	if (virtual_base == MAP_FAILED) 
	{
		printf("ERROR: mmap() failed...\n");
		close(fd);
		return -1;
	}
	
	leds_addr = virtual_base + CUSTOM_LEDS_0_BASE;
	switch_addr = virtual_base + SWITCH_BASE;

	uint32_t  state;
    state = *(uint32_t *)switch_addr;
	*(uint32_t *)leds_addr = state;
	
	// Clean up our memory mapping and exit
	if (munmap(virtual_base, REGS_SPAN) != 0){
		printf("ERROR: munmap() failed...\n");
		close(fd);
		return -1;
	}

	close(fd);
}



int serialInit(){

    int fd;           //device file id
    //------------------------------- Opening the Serial Port -------------------------------
    fd = open("/dev/ttyS0",O_RDWR | O_NOCTTY);    // ttyUSB0 is the FT232 based USB2SERIAL Converter 
    if(fd == -1)                        // Error Checking 
    printf("Error while opening the device\n");

    //---------- Setting the Attributes of the serial port using termios structure ---------
    struct termios SerialPortSettings;  // Create the structure                          
    tcgetattr(fd, &SerialPortSettings); // Get the current attributes of the Serial port
    // Setting the Baud rate
    cfsetispeed(&SerialPortSettings,B115200); // Set Read  Speed as 115200                      
    cfsetospeed(&SerialPortSettings,B115200); // Set Write Speed as 115200                  

    SerialPortSettings.c_cflag &= ~PARENB;   // Disables the Parity Enable bit(PARENB),So No Parity   
    SerialPortSettings.c_cflag &= ~CSTOPB;   // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
    SerialPortSettings.c_cflag &= ~CSIZE;    // Clears the mask for setting the data size             
    SerialPortSettings.c_cflag |=  CS8;      // Set the data bits = 8                                 
    SerialPortSettings.c_cflag &= ~CRTSCTS;       // No Hardware flow Control                         
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; // Enable receiver,Ignore Modem Control lines        
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);  // Disable XON/XOFF flow control both i/p and o/p 
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode 
    SerialPortSettings.c_oflag &= ~OPOST;//No Output Processing
    // Setting Time outs 
    SerialPortSettings.c_cc[VMIN] = 13; // Read at least 10 characters 
    SerialPortSettings.c_cc[VTIME] = 0; // Wait indefinetly  

    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) // Set the attributes to the termios structure
    printf("Error while setting attributes \n");

    return fd;
}
 
int main()
{
    int fd = serialInit();
    
    //------------------------------- Read data from serial port -----------------------------
    char read_buffer[32];   // Buffer to store the data received 
    char write_buffer[32];             
    int  bytes_read = 0;    // Number of bytes read by the read() system call 
    int bytes_written = 0;  // Number of bytes written

    char switches[]= "switch";
    char tmp[strlen(switches)];

    tcflush(fd, TCIFLUSH);   // Discards old data in the rx buffer          
  
    bytes_read = read(fd,read_buffer,32); // Read the data
    strcpy(write_buffer, read_buffer);
    bytes_written=write(fd,write_buffer,sizeof(write_buffer)); // Send it 
    //printf("bytes written: ");
    //printf("%d\n",bytes_written);
    
    strncpy(tmp, read_buffer, strlen(switches));

    printf("%s\n", tmp);
    printf("%s\n", switches);

    if (!strcmp(tmp, switches)){
        printf("\nkapcsolo\n");
        printf("Peripherals coming:\n");
        peripherals();        
    }
    else{
        printf("not the same\n");
    }
    

    close(fd); // Close the serial port
    return 0;
}