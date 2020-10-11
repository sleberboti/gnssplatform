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
    uint32_t CounterRead();
    bool     get_icm_addr_base(uint32_t *addr);
    bool     get_ms_addr_base(uint32_t *addr);
	bool     get_ublox_addr_base(uint32_t *addr);
    bool     get_led_addr_base(uint32_t *addr);
    bool     get_time_addr_base(uint32_t *addr);
    bool     get_counter_addr_base(uint32_t *addr);
    bool     get_mm_addr_base(uint32_t *addr);
    bool     get_imu_uart_addr_base(uint32_t *addr);
    bool     IMU_Uart_Read(uint32_t *imu_data);
    bool     IMU_Uart_Write(char *string, int len);
	bool     release_mem();

protected:
    bool     m_bInitSuccess;
    int      m_file_mem;
    void     *virtual_base;

    uint8_t  *m_led_base;
    uint8_t  *m_sw_base;
	uint8_t  *m_ublox_i2c_base;
    uint8_t  *m_time_base;
    uint8_t  *m_counter_base;
    uint8_t  *m_mm_i2c_base;
    uint8_t  *m_ms_i2c_base;
    uint8_t  *m_icm_i2c_base;
    uint8_t  *m_imu_uart_base;
	
    bool     Init();

};

#endif // FPGA_H
