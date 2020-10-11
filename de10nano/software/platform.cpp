//===================================include source files========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

// timer related
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
// save to file
#include <fstream>
#include <string>

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
MS5611       *pms;            //point to MS5611 - Altimeterinstance
IMU_ICM      *picm;        	  //point to ICM - IMU instance
UBLOX        *publox;         //point to UBLOX instance
MM           *pmm;            //point to MM - Magnetometer instance
IMU_CV5      *pimucv5;        //point to CV5 - IMU instance
HostCom      my_host;
sensorPacket sensor;
HPS          my_hps;         // hps instance	
FPGA         my_fpga;        // fpga instance


/**************************************************************************
Function     : Measurement functions for the sensors
**************************************************************************/
void imu_cv5Meas(int measDur, double measFreq, bool imu_cv5){
	if (imu_cv5){
		for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{measDur}; now = std::chrono::steady_clock::now()){
			auto before = std::chrono::high_resolution_clock::now();

			sensor=pimucv5->pollIMU();
			my_host.writer(sensor);

			auto elapsed = std::chrono::high_resolution_clock::now() - before;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			usleep((1000000/measFreq)-microseconds);
		}
	}	
}

void imu_icmMeas(int measDur, double measFreq, uint32_t time_addr, bool imu_icm){	
	if (imu_icm){

		uint32_t subsec;
		uint32_t prev_sec;

		for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{measDur}; now = std::chrono::steady_clock::now()){
			auto before = std::chrono::high_resolution_clock::now();

			printf(" Time addr: %x ", time_addr);
			subsec = my_fpga.TimeRead();
			prev_sec = my_fpga.CounterRead();
			sensor=picm->getICMdata();
			sensor.icm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
			printf("IMU ICM sensor has been read\n");
			my_host.writer(sensor);

			auto elapsed = std::chrono::high_resolution_clock::now() - before;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			usleep((1000000/measFreq)-microseconds);
		}
	}	
}

void mmMeas(int measDur, double measFreq, uint32_t time_addr, bool mm){	
	if (mm){
		uint32_t subsec;
		uint32_t prev_sec;
		for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{measDur}; now = std::chrono::steady_clock::now()){
			auto before = std::chrono::high_resolution_clock::now();

			printf(" Time addr: %x ", time_addr);		
			subsec = my_fpga.TimeRead();
			prev_sec = my_fpga.CounterRead();	
			sensor=pmm->getMMdata();	
			sensor.mm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
			my_host.writer(sensor);

			auto elapsed = std::chrono::high_resolution_clock::now() - before;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			usleep((1000000/measFreq)-microseconds);
		}
	}	
}

void msMeas(int measDur, double measFreq, uint32_t time_addr, bool ms){	
	if (ms){
		uint32_t subsec;
		uint32_t prev_sec;
		for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{measDur}; now = std::chrono::steady_clock::now()){
			auto before = std::chrono::high_resolution_clock::now();

			printf(" Time addr: %x ", time_addr);				
			subsec = my_fpga.TimeRead();
			prev_sec = my_fpga.CounterRead();
			sensor=pms->getMSdata();
			sensor.ms5611_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);	
			my_host.writer(sensor);

			auto elapsed = std::chrono::high_resolution_clock::now() - before;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			usleep((1000000/measFreq)-microseconds);
		}
	}	
}

void ubloxMeas(int measDur, double measFreq, bool ublox){	
	if (ublox){
		for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{measDur}; now = std::chrono::steady_clock::now()){
			auto before = std::chrono::high_resolution_clock::now();
			bool stop = false;
			while(!stop){
				sensor=publox->getUbloxBuffer();
				if(sensor.readSfrbx == true || sensor.readMeasx == true || sensor.readPvt == true) stop = true;
			}
			sensor.readPvt   = false;
			sensor.readMeasx = false;
			sensor.readSfrbx = false;
			sensor.globitow = sensor.iTOW/1000; // updating the global time
			my_host.writer(sensor);

			auto elapsed = std::chrono::high_resolution_clock::now() - before;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			usleep((1000000/measFreq)-microseconds);
		}
	}	
}

/**************************************************************************
Function     : The main function
**************************************************************************/

int main(int argc, char **argv)
{	
	bool hostcommand  = false;	   // request based measurements
	bool hostcommand_init = false;     // one-time request for a measurent sequence.
	bool localcommand = false;
	bool timecommand  = false;
	
	if (!strcmp(argv[1], "host")) {
        hostcommand=true;
		printf("HOST\n");
    }
	if (!strcmp(argv[1], "host_init")) {
        hostcommand_init=true;
		printf("HOST_init\n");
    }
	if (!strcmp(argv[1], "local")) {
        localcommand=true;		
		printf("LOCAL\n");
    }
	if (!strcmp(argv[1], "time")) {
        timecommand=true;		
		printf("TIME\n");
    }

	printf("\n|| Postitioning Platform starts ||\n\n");		
	
	my_fpga.LedSet(0xff);
	uint32_t imu_uart_addr;
	uint32_t ublox_i2c_addr;
	uint32_t mm_i2c_addr;
	uint32_t ms_i2c_addr;
	uint32_t icm_i2c_addr;
	uint32_t led_addr;
	uint32_t time_addr;
	uint32_t counter_addr;
	my_fpga.get_ms_addr_base(&ms_i2c_addr);
	my_fpga.get_icm_addr_base(&icm_i2c_addr);
	my_fpga.get_ublox_addr_base(&ublox_i2c_addr);
	my_fpga.get_led_addr_base(&led_addr);	
	my_fpga.get_counter_addr_base(&counter_addr);
	my_fpga.get_time_addr_base(&time_addr);
	my_fpga.get_mm_addr_base(&mm_i2c_addr);
	my_fpga.get_imu_uart_addr_base(&imu_uart_addr);
		
    //printf("Time: %zu\n",my_fpga.TimeRead());

	pimucv5 = new IMU_CV5((uint32_t)imu_uart_addr);
	pimucv5->pollIMU();

	publox = new UBLOX((uint32_t)ublox_i2c_addr, 0x42); // 0x42 -> device address

	pmm = new MM((uint32_t)mm_i2c_addr, 0xE); // 0xE -> device address
	pmm->initialize();

	pms = new MS5611((uint32_t)ms_i2c_addr, 0x77); // 0x77 -> device address	
	pms->initialize();

	picm = new IMU_ICM((uint32_t)icm_i2c_addr, 0x69);
	picm->initialize();

	sensor = publox->getPvtBuffer();
	printf("\nGPS itow [ms]: %x %d\n",sensor.iTOW, sensor.iTOW);
	sensor.globitow = sensor.iTOW/1000; // ublox time in sec

	pimucv5->updateGPStime(uint32_t(sensor.globitow));
	usleep(1000000);
	pimucv5->updateGPStime(uint32_t(sensor.globitow));

	std::queue<std::string> myqueue;	
	std::thread listenerThread(&HostCom::listener, &my_host, std::ref(myqueue), hostcommand_init);

	// host initialised measurement procedure
	if(hostcommand_init){

		for(int i=0; i<10; i++){
			if(my_fpga.CounterRead()>60000000 || my_fpga.CounterRead()<40000000){
				printf("\t Smth is wrong, maybe ublox does not have a fixed signal\n");
				break;
			}
		}

		bool imu_cv5 = false;
		bool imu_icm = false;
		bool ublox   = false;
		bool ms      = false;
		bool mm      = false;

		while (myqueue.size()==0){
			usleep(1);
		}
		usleep(1000000); // 1 sec wait

		double measFreq;
		int measDur;
				
		while (!myqueue.empty()){
			std::string element = myqueue.front();
			myqueue.pop();
			if(isdigit(element[0])){
				if(std::stod(element)<10){
					measFreq = std::stod(element);
				}
				else{
					measDur = std::stoi(element);
				}
			} 
			//if(isdigit(element[0])) measFreq = std::stod(element);
			if(element=="imu_cv5")  imu_cv5 = true;
			if(element=="imu_icm")  imu_icm = true;
			if(element=="ublox")    ublox   = true;
			if(element=="mm")       mm      = true;
			if(element=="ms")       ms      = true;
		}

		printf("%d %d %d %d %d %f %d\n", imu_cv5, imu_icm, ublox, mm, ms, measFreq, measDur);

		std::thread imu_cv5Thread(imu_cv5Meas, measDur, measFreq, imu_cv5);
		std::thread imu_icmThread(imu_icmMeas, measDur, measFreq, time_addr, imu_icm);
		std::thread ubloxThread(ubloxMeas, measDur, measFreq, ublox);
		std::thread mmThread(mmMeas, measDur, measFreq, time_addr, mm);
		std::thread msThread(msMeas, measDur, measFreq, time_addr, ms);

		imu_cv5Thread.join();
		imu_icmThread.join();
		ubloxThread.join();
		mmThread.join();
		msThread.join();		 
	}

	// used for measurements without a host, usually for test and development purposes 
	if (localcommand){
		uint32_t subsec;
		uint32_t prev_sec;

		for(int i=0; i<10; i++){
			if(my_fpga.CounterRead()>55000000 || my_fpga.CounterRead()<45000000){
				printf("\t Smth is wrong, maybe ublox does not have a fixed signal\n");
				break;
			}
		}

		/// get mm data plus timestamp
		sensor = pmm-> getMMdata();
		subsec = my_fpga.TimeRead();
		prev_sec = my_fpga.CounterRead();		
		sensor.mm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
		printf("MM data timestamp [ns]: %llu\n", sensor.mm_itow);
		///

		/// get imu cv5 data plus timestamp
		sensor = pimucv5->pollIMU();
		printf("IMU timestamp [ns]:     %llu\n", sensor.tow);
		///

		/// get MS5611 data plus timestamp
		sensor=pms->getMSdata();		
		subsec = my_fpga.TimeRead();
		prev_sec = my_fpga.CounterRead();
		sensor.ms5611_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
		printf("MS5611 timestamp [ns]:  %llu\n", sensor.ms5611_itow);
		///

		/// get IMU ICM data plus timestamp
		sensor=picm->getICMdata();		
		subsec = my_fpga.TimeRead();
		prev_sec = my_fpga.CounterRead();
		sensor.icm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
		printf("IMU ICM timestamp [ns]: %llu\n", sensor.icm_itow);
		
		/// get ublox data
		while(true)	{
			sensor = publox->getUbloxBuffer();
		}

	}
	
	// used for host dictated measurements
	if(hostcommand == true){
		uint32_t subsec;
		uint32_t prev_sec;

		for(int i=0; i<10; i++){
			if(my_fpga.CounterRead()>55000000 || my_fpga.CounterRead()<45000000){
				printf("\t Smth is wrong, maybe ublox does not have a fixed signal\n");
				break;
			}
		}

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
					
					printf("IMU CV5 Sensor has been read\n");
					my_host.writer(sensor);
				}
				if(element=="imu_icm"){
					element="";
					printf(" Time addr: %x ", time_addr);
					subsec = my_fpga.TimeRead();
					prev_sec = my_fpga.CounterRead();
					sensor=picm->getICMdata();
					sensor.icm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
					printf("IMU ICM sensor has been read\n");
					my_host.writer(sensor);
				}
				if(element=="mm"){
					element="";			
					printf(" Time addr: %x ", time_addr);		
					subsec = my_fpga.TimeRead();
					prev_sec = my_fpga.CounterRead();	
					sensor=pmm->getMMdata();	
					sensor.mm_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);
					
					printf("MM Sensor has been read\n");
					my_host.writer(sensor);
				}
				if(element=="ublox"){
					element="";
					bool stop = false;
					while(!stop){
						sensor=publox->getUbloxBuffer();
						//printf("%d %d %d\n", sensor.readPvt, sensor.readMeasx, sensor.readSfrbx);
						if(sensor.readSfrbx == true || sensor.readMeasx == true || sensor.readPvt == true) stop = true;
					}

					sensor.readPvt   = false;
					sensor.readMeasx = false;
					sensor.readSfrbx = false;
					
					printf("Ublox buffer has been read\n");
					my_host.writer(sensor);
				}
				if(element=="ms"){
					printf("MS5611 is getting read\n");
					element="";	
					printf(" Time addr: %x ", time_addr);				
					subsec = my_fpga.TimeRead();
					printf("MS5611 1\n");
					prev_sec = my_fpga.CounterRead();
					printf("MS5611 2\n");
					sensor=pms->getMSdata();
					printf("MS5611 3\n");
					sensor.ms5611_itow = int(sensor.globitow)*pow(10,9)+round(subsec*pow(10,8)*10/prev_sec);			
					printf("MS5611 has been read\n");
					my_host.writer(sensor);
					//break;
				}
			}
		}
	}

	// measurement for the thesis
	if (timecommand){			
		printf("Time addr: %x\n", time_addr);
		printf("Counter addr: %x\n", counter_addr);
		uint32_t counter;
		std::time_t t;
		int t_int=0;
		int t_prev_int=0;
		
		bool stop = false;
		for (int i=0; i<10; i++){			
			int starttime = int(std::time(0));
			std::ofstream myfile;
			std::string filename;
			filename = "meas" + std::to_string(starttime) + ".csv";
			myfile.open(filename);
			stop = false;
			while(!stop){
				if(my_fpga.CounterRead() != 0){
					counter = my_fpga.CounterRead();				
					t = std::time(0);  // t is an integer type
					t_int = int(t);
					if(t_int>t_prev_int){
						std::cout << i*4 << ". hour - " <<  t_int <<  "," << counter << "\n";
						myfile  << t_int <<  "," << counter << "\n";
					}
					t_prev_int = t_int;
				}	
				if((t_int - starttime)>=(4*3600-1)){
					myfile.close();
					stop = true;
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