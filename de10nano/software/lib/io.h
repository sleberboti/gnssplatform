#ifndef __IO_H__
#define __IO_H__

/* IO Header file for Nios II Toolchain */
#include <stdint.h>

/* Native bus access functions */

#define __IO_CALC_ADDRESS_NATIVE(BASE, REGNUM) \
	(*(((uint32_t*)BASE) + (REGNUM)))

#define IORD(BASE, REGNUM) \
	(*(((uint32_t*)BASE) + (REGNUM)))
#define IOWR(BASE, REGNUM, DATA) \
	(*(((uint32_t*)BASE) + (REGNUM))=(DATA))

#endif /* __IO_H__ */