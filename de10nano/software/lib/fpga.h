#ifndef FPGA_H
#define FPGA_H

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

class FPGA
{
public:
    FPGA();
    ~FPGA();

    bool     LedSet(int mask);
	bool     SwitchRead(uint32_t *mask);
    uint32_t TimeRead();
	bool     get_ublox_addr_base(uint32_t *addr);
    bool     get_led_addr_base(uint32_t *addr);
    bool     get_time_addr_base(uint32_t *addr);
    bool     get_mm_addr_base(uint32_t *addr);
	bool     release_mem();

protected:
    bool     m_bInitSuccess;
    int      m_file_mem;
    void     *virtual_base;

    uint8_t  *m_led_base;
    uint8_t  *m_sw_base;
	uint8_t  *m_ublox_i2c_base;
    uint8_t  *m_time_base;
    uint8_t  *m_mm_i2c_base;
	
    bool     Init();

};

#endif // FPGA_H
