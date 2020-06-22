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
#include "hostcom.h"
#include "imu_cv5.h"
#include "imu_icm.h"
#include "ms5611.h"
#include "def.h"

//==========================define global variable================================================
MS5611     *pms;             //point to MS5611 - Altimeterinstance
//ICM      *picm;        	 //point to ICM - IMU instance
UBLOX      *publox;          //point to UBLOX instance
MM         *pmm;             //point to MM - Magnetometer instance
IMU_CV5    *pimucv5;         //point to CV5 - IMU instance


/**************************************************************************
Function     : The main function
parameter    : 
return value :
**************************************************************************/

int main(int argc, char **argv)
{	
	bool hostcommand=false;	
	bool localcommand=false;
	
	if (!strcmp(argv[1], "host")) {
        hostcommand=true;
    }
	if (!strcmp(argv[1], "local")) {
        localcommand=true;
    }

	printf("\n|| Postitioning Platform starts ||\n\n");

	// hps instance
	HPS  my_hps;
	// fpga instance
	FPGA my_fpga;			
	
	my_fpga.LedSet(0xff);
	uint32_t imu_uart_addr;
	uint32_t ublox_i2c_addr;
	uint32_t mm_i2c_addr;
	uint32_t ms_i2c_addr;
	uint32_t icm_i2c_addr;
	uint32_t led_addr;
	uint32_t time_addr;
	my_fpga.get_ms_addr_base(&ms_i2c_addr);
	my_fpga.get_icm_addr_base(&icm_i2c_addr);
	my_fpga.get_ublox_addr_base(&ublox_i2c_addr);
	my_fpga.get_led_addr_base(&led_addr);
	my_fpga.get_time_addr_base(&time_addr);
	my_fpga.get_mm_addr_base(&mm_i2c_addr);
	my_fpga.get_imu_uart_addr_base(&imu_uart_addr);
		
    //printf("Time: %zu\n",my_fpga.TimeRead());

	pimucv5 = new IMU_CV5((uint32_t)imu_uart_addr);
	publox = new UBLOX((uint32_t)ublox_i2c_addr, 0x42); // 0x42 -> device address
	publox->initialize();

	/* pmm = new MM((uint32_t)mm_i2c_addr, 0xE); // 0xE -> device address
	pmm->initialize(); */

	pms = new MS5611((uint32_t)ms_i2c_addr, 0x77); // 0x77 -> device address	
	pms->initialize();
	
	//extern sensorPacket sensor;
	sensorPacket sensor;
	
	std::queue<std::string> myqueue;
	HostCom my_host;	
	std::thread listenerThread(&HostCom::listener, &my_host, std::ref(myqueue), hostcommand);


	if (localcommand){

		//pmm-> getMMdata();
		//publox->getUbloxBuffer();
		
		/* while (true){
			pmm-> getMMdata();
			usleep(1000000);
		} */
	}
	
	if(hostcommand){
		// hostcom instance
		while(true){
			if (!myqueue.empty())
			{	
				std::string element = myqueue.front();
				printf("Queue size: %d\n", myqueue.size());					
				myqueue.pop();			
				printf("Queue size: %d\n", myqueue.size());	
				if(element=="imu_cv5"){
					element="";
					sensor=pimucv5->pollIMU();
					
					printf("IMU Sensor has been read\n");
					my_host.writer(sensor);
					//break;
				}
				if(element=="mm"){
					element="";
					sensor=pmm->getMMdata();;
					
					printf("MM Sensor has been read\n");
					my_host.writer(sensor);
					//break;
				}
				if(element=="ublox_pvt"){
					element="";
					sensor=publox->getUbloxBuffer();
					
					printf("Ublox PVT has been read\n");
					my_host.writer(sensor);
					//break;
				}
				if(element=="ms"){
					element="";
					sensor=pms->getMSdata();
					
					printf("MS5611 has been read\n");
					my_host.writer(sensor);
					//break;
				}
			}
		}
	}
	

	
	//usleep(1000000);	
	//std::cout << std::endl << "senzor: " << sensor.tow << std::endl;
	//pimucv5->pollIMU2();
	
	/* while(true){
		pimucv5->pollIMU();
		usleep(100000000000000);
		printf("\n");
	}	 */

	//pimucv5->send_imu_uart_data();
	//pimucv5->get_imu_uart_data();
	
	//pmm->getMMdata();

	/* publox = new UBLOX((uint32_t)ublox_i2c_addr, 0x42); // 0x42 -> device address
	publox->initialize();
	publox->readLenght();
	publox->getPVT(); */
	//publox->getUbloxBuffer();
	

	my_fpga.LedSet(0x06);
	my_fpga.LedSet(0x07);
	my_fpga.LedSet(0x08);
	my_fpga.LedSet(0x09);
	my_fpga.LedSet(0x10);
	
	printf("Cleaning up\n");
	free(publox);
	free(pmm);
	free(pimucv5);
	listenerThread.detach();


	return 0;
}