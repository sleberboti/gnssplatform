#ifndef _ALTERA_HPS_0_H_
#define _ALTERA_HPS_0_H_

/*
 * This file was automatically generated by the swinfo2header utility.
 * 
 * Created from SOPC Builder system 'soc_system' in
 * file './soc_system.sopcinfo'.
 */

/*
 * This file contains macros for module 'hps_0' and devices
 * connected to the following master:
 *   h2f_lw_axi_master
 * 
 * Do not include this header file and another header file created for a
 * different module or master group at the same time.
 * Doing so may result in duplicate macro names.
 * Instead, use the system header file which has macros with unique names.
 */

/*
 * Macros for device 'ublox_i2c', class 'i2c_opencores'
 * The macros are prefixed with 'UBLOX_I2C_'.
 * The prefix is the slave descriptor.
 */
#define UBLOX_I2C_COMPONENT_TYPE i2c_opencores
#define UBLOX_I2C_COMPONENT_NAME ublox_i2c
#define UBLOX_I2C_BASE 0x0
#define UBLOX_I2C_SPAN 32
#define UBLOX_I2C_END 0x1f
#define UBLOX_I2C_IRQ 0

/*
 * Macros for device 'switch', class 'altera_avalon_pio'
 * The macros are prefixed with 'SWITCH_'.
 * The prefix is the slave descriptor.
 */
#define SWITCH_COMPONENT_TYPE altera_avalon_pio
#define SWITCH_COMPONENT_NAME switch
#define SWITCH_BASE 0x20
#define SWITCH_SPAN 16
#define SWITCH_END 0x2f
#define SWITCH_BIT_CLEARING_EDGE_REGISTER 0
#define SWITCH_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SWITCH_CAPTURE 0
#define SWITCH_DATA_WIDTH 4
#define SWITCH_DO_TEST_BENCH_WIRING 0
#define SWITCH_DRIVEN_SIM_VALUE 0
#define SWITCH_EDGE_TYPE NONE
#define SWITCH_FREQ 50000000
#define SWITCH_HAS_IN 1
#define SWITCH_HAS_OUT 0
#define SWITCH_HAS_TRI 0
#define SWITCH_IRQ_TYPE NONE
#define SWITCH_RESET_VALUE 0

/*
 * Macros for device 'custom_leds_0', class 'custom_leds'
 * The macros are prefixed with 'CUSTOM_LEDS_0_'.
 * The prefix is the slave descriptor.
 */
#define CUSTOM_LEDS_0_COMPONENT_TYPE custom_leds
#define CUSTOM_LEDS_0_COMPONENT_NAME custom_leds_0
#define CUSTOM_LEDS_0_BASE 0x30
#define CUSTOM_LEDS_0_SPAN 8
#define CUSTOM_LEDS_0_END 0x37

/*
 * Macros for device 'time_sync_0', class 'time_sync'
 * The macros are prefixed with 'TIME_SYNC_0_'.
 * The prefix is the slave descriptor.
 */
#define TIME_SYNC_0_COMPONENT_TYPE time_sync
#define TIME_SYNC_0_COMPONENT_NAME time_sync_0
#define TIME_SYNC_0_BASE 0x38
#define TIME_SYNC_0_SPAN 8
#define TIME_SYNC_0_END 0x3f

/*
 * Macros for device 'magnetometer_i2c', class 'i2c_opencores'
 * The macros are prefixed with 'MAGNETOMETER_I2C_'.
 * The prefix is the slave descriptor.
 */
#define MAGNETOMETER_I2C_COMPONENT_TYPE i2c_opencores
#define MAGNETOMETER_I2C_COMPONENT_NAME magnetometer_i2c
#define MAGNETOMETER_I2C_BASE 0x40
#define MAGNETOMETER_I2C_SPAN 32
#define MAGNETOMETER_I2C_END 0x5f
#define MAGNETOMETER_I2C_IRQ 1


#endif /* _ALTERA_HPS_0_H_ */