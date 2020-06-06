//===================================include source files========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "fpga.h"
#include "hps.h"
#include "ublox.h"	
#include "mm.h"

//==========================define global variable================================================
UBLOX *publox;      //point to UBLOX instance
MM    *pmm;         //point to MM - Magnetometer instance

/**************************************************************************
Function     : The main function
parameter    : 
return value :
**************************************************************************/
int main(int argc, char **argv)
{
	// hps instance
	HPS  my_hps;
	// fpga instance
	FPGA my_fpga;			
	
	my_fpga.LedSet(0xff);
	uint32_t ublox_i2c_addr;
	uint32_t mm_i2c_addr;
	uint32_t led_addr;
	uint32_t time_addr;
	my_fpga.get_ublox_addr_base(&ublox_i2c_addr);
	my_fpga.get_led_addr_base(&led_addr);
	my_fpga.get_time_addr_base(&time_addr);
	my_fpga.get_mm_addr_base(&mm_i2c_addr);
		
    printf("\tTime: %zu\n",my_fpga.TimeRead());

	pmm = new MM((uint32_t)mm_i2c_addr, 0xE); // 0xE -> device address
	pmm->initialize();
	pmm->getMMdata();

	publox = new UBLOX((uint32_t)ublox_i2c_addr, 0x42); // 0x42 -> device address
	publox->initialize();
	publox->getUbloxBuffer();

	

	my_fpga.LedSet(0x06);
	my_fpga.LedSet(0x07);
	my_fpga.LedSet(0x08);
	my_fpga.LedSet(0x09);
	my_fpga.LedSet(0x10);
	
	free(publox);
	free(pmm);
	
	return 0;
}//end main