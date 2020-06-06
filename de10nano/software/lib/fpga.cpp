#include "fpga.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>


#include <sys/mman.h>
#include <sys/types.h>

#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include <iostream>

// QSyS dependent address -- Copied from generated "hps_0.h"
#define FPGA_MM_I2C_BASE           0x40    // 32 byte
#define FPGA_TIME_BASE             0x38    // 8  byte
#define FPGA_LED_PIO_BASE          0x30    // 8  byte
#define FPGA_SW_PIO_BASE    	   0x20    // 16 byte
#define FPGA_UBLOX_I2C_BASE		   0x00    // 32 byte
	

#define IORD(base, index)                             (*( ((uint32_t *)base)+index))
#define IOWR(base, index, data)                       (*(((uint32_t *)base)+index) = data)
// ///////////////////////////////////////
// memory map
#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
// end
// ///////////////////////////////////////////////////

FPGA::FPGA()
{
    m_bInitSuccess = Init();
}

FPGA::~FPGA()
{
	printf("exit ~FPGA\n");
    //close(m_file_mem);
}

bool FPGA::Init()
{
    bool bSuccess = false;

    m_file_mem = open( "/dev/mem", ( O_RDWR | O_SYNC ) );
    if (m_file_mem != -1){
        virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, m_file_mem, HW_REGS_BASE );
        if (virtual_base == MAP_FAILED){
        }else{
            m_led_base			  = (uint8_t *)virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + FPGA_LED_PIO_BASE)  	   & (unsigned long)(HW_REGS_MASK));
            m_sw_base             = (uint8_t *)virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + FPGA_SW_PIO_BASE)         & (unsigned long)(HW_REGS_MASK));
			m_ublox_i2c_base	  =	(uint8_t *)virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + FPGA_UBLOX_I2C_BASE)      & (unsigned long)(HW_REGS_MASK));
            m_time_base           =	(uint8_t *)virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + FPGA_TIME_BASE)           & (unsigned long)(HW_REGS_MASK));
            m_mm_i2c_base         =	(uint8_t *)virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + FPGA_MM_I2C_BASE)         & (unsigned long)(HW_REGS_MASK));

            bSuccess = true;
        }
        close(m_file_mem);
    }
	printf("memory map success\n");

    return bSuccess;
}


bool FPGA::LedSet(int mask){
    if (!m_bInitSuccess)
        return false;
    //std::cout << "LEDBASE" << &m_led_base << std::endl;
	IOWR(m_led_base,0,mask);
    return true;
}

bool FPGA::SwitchRead(uint32_t *mask){
    if (!m_bInitSuccess)
        return false;
    *mask = IORD(m_sw_base,0);
    return true;
}

uint32_t FPGA::TimeRead(){
    if (!m_bInitSuccess)
        return false;
    uint32_t *counter;
    *counter = IORD(m_time_base,0);
    return *counter;
}

bool FPGA::get_ublox_addr_base(uint32_t *addr)
{
	if (!m_bInitSuccess)
        return false;    
	*addr = (uint32_t)m_ublox_i2c_base;
	return true;	
}

bool FPGA::get_mm_addr_base(uint32_t *addr)
{
	if (!m_bInitSuccess)
        return false;    
	*addr = (uint32_t)m_mm_i2c_base;
	return true;	
}

bool FPGA::get_time_addr_base(uint32_t *addr)
{
	if (!m_bInitSuccess)
        return false;    
	*addr = (uint32_t)m_time_base;
	return true;	
}

bool FPGA::get_led_addr_base(uint32_t *addr)
{
	if (!m_bInitSuccess)
        return false;    
	*addr = (uint32_t)m_led_base;
	return true;	
}

bool FPGA::release_mem()
{
	if (!m_bInitSuccess)
        return false;
    munmap(virtual_base, HW_REGS_SPAN);
    return true;
}