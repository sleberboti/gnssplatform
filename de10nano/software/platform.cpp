//===================================include source files========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include "def.h"
#include "hostcom.h"
#include "fpga.h"
#include "hps.h"
#include "ublox.h"	
#include "mm.h"
#include "imu_cv5.h"
#include "imu_icm.h"
#include "ms5611.h"

//==========================define global variable================================================
MS5611     *pms;             //point to MS5611 - Altimeterinstance
IMU_ICM    *picm;        	 //point to ICM - IMU instance
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
	sensorPacket sensor;
	bool hostcommand=false;	
	bool localcommand=false;
	uint32_t globitow;
	
	if (!strcmp(argv[1], "host")) {
        hostcommand=true;
		printf("HOST\n");
    }
	if (!strcmp(argv[1], "local")) {
        localcommand=true;		
		printf("LOCAL\n");
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
	pimucv5->pollIMU();

	publox = new UBLOX((uint32_t)ublox_i2c_addr, 0x42); // 0x42 -> device address
	publox->initialize();

	//sensor=publox->getUbloxBuffer();
	//printf("%d\n", sensor.iTOW);

	pmm = new MM((uint32_t)mm_i2c_addr, 0xE); // 0xE -> device address
	pmm->initialize();

	/*
	pms = new MS5611((uint32_t)ms_i2c_addr, 0x77); // 0x77 -> device address	
	pms->initialize(); */

	//picm = new IMU_ICM((uint32_t)icm_i2c_addr, 0x69);
	//picm->initialize();
	//printf("init return:%d\n",picm->initialize());

	printf("\n");
	sensor=publox->getUbloxBuffer();
	printf("\nGPS itow: %x %d %x %d\n",sensor.iTOW, sensor.iTOW, sensor.iTOW/1000, sensor.iTOW/1000);
	uint32_t ublox_itow = sensor.iTOW/1000; // ublox time in sec
	globitow = ublox_itow;

	printf("ublox itow %x %d\n", ublox_itow, ublox_itow);
	pimucv5->updateGPStime(uint32_t(ublox_itow));
	//sensor = pimucv5->pollIMU();



	/* printf("get imu data? current gps week value?:\n");
	pimucv5->get_imu_uart_data();
	printf("get imu data? current gps second value?:\n");
	pimucv5->get_imu_uart_data(); */
	/* sensor = pimucv5->pollIMU();
	printf("IMU itow %f\n", sensor.tow); */

	std::queue<std::string> myqueue;
	HostCom my_host;	
	std::thread listenerThread(&HostCom::listener, &my_host, std::ref(myqueue), hostcommand);
	

	if (localcommand){
		double timeofweek;
		double previous;
		uint32_t subsec;
		
		printf("Time addr: %x\n", time_addr);
		sensor = pmm-> getMMdata(globitow);
		subsec = my_fpga.TimeRead();
		sensor.mm_itow = globitow*pow(10,7)+subsec;
		printf("PVT itow: %x %d, Counter: %d, Sum: %f\n", globitow, globitow, subsec, sensor.mm_itow);

		printf("Time addr: %x\n", time_addr);
		subsec = my_fpga.TimeRead();
		printf("\t\t\tSubsec_fpga: %d\n", subsec);
		sensor = pimucv5->pollIMU();
		printf("IMU itow %f\n", sensor.tow);
		subsec = my_fpga.TimeRead();
		printf("Subsec_fpga: %d\n", subsec);

		uint32_t subsec_max=0;
		uint32_t subsec_min=0xFFFFFFFF;

		while (true){
			
			sensor=publox->getUbloxBuffer();
			printf("\nGPS itow: %x %d %x %d %x\n",sensor.iTOW, sensor.iTOW, sensor.iTOW/1000, sensor.iTOW/100, sensor.nano);
			printf("Time addr: %x\n", time_addr);
			double gpstow = sensor.iTOW/100;
			subsec = my_fpga.TimeRead();
			printf("\t\t\tSubsec_fpga: %d\n", subsec);
			//printf("Diff: %d", abs(sensor.iTow/100-subsec))
			sensor = pimucv5->pollIMU();
			printf("\tIMU itow %f\n", sensor.tow); 
			printf("Time addr: %x\n", time_addr);
			subsec = my_fpga.TimeRead();
			printf("\t\t\tSubsec_fpga: %d\n", subsec);

			if(subsec<subsec_min){
				subsec_min = subsec;
			}
			if(subsec>subsec_max){
				subsec_max = subsec;
			}
			
			/* printf("subsec_max: %x %d\n", subsec_max, subsec_max);
			printf("subsec_min: %x %d\n", subsec_min, subsec_min); */


			//printf("Time addr: %x\n", time_addr);
			//uint32_t addr = time_addr;
			
			//int counter = my_fpga.TimeRead();
			
			//printf("Time: %d %x\n", counter);
			//pmm-> getMMdata();
			//usleep(1000000);
			//picm->getICMdata();
			//picm->readSensor();
			//printf("%f %f %f\n",picm->getAccelX_mss(),picm->getAccelY_mss(),picm->getAccelZ_mss());
			usleep(50000); 
			
			// 1 sec
			//usleep(1000000); 
			printf("%d", sensor.tow-gpstow);
			printf("\n\n");
		}
	}
	
	if(hostcommand == true){
		// hostcom instance
		while(true){
			if (!myqueue.empty())
			{	
				std::string element = myqueue.front();
				printf("Queue size: %d\n", myqueue.size());					
				myqueue.pop();			
				//printf("Queue size: %d\n", myqueue.size());	
				if(element=="imu_cv5"){
					element="";
					sensor=pimucv5->pollIMU();
					
					printf("IMU CV5 Sensor has been read\n");
					my_host.writer(sensor);
					//break;
				}
				if(element=="imu_icm"){
					element="";
					sensor=picm->getICMdata();
					
					printf("IMU ICM sensor has been read\n");
					my_host.writer(sensor);
					//break;
				}
				if(element=="mm"){
					element="";
					sensor=pmm->getMMdata(globitow);;
					
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