#ifndef _CFSENSOR_H_
#define _CFSENSOR_H_
#include "hal_sys.h"

#define CFS_IO_SDA_IN			PBout(9) = 1//__disable_fiq();GPIOB->CRH &= 0xFFFFFF0F;GPIOB->CRH |= 0x04<<4;__enable_irq();
#define CFS_IO_SDA_OUT		//__disable_fiq();GPIOB->CRH &= 0xFFFFFF0F;GPIOB->CRH |= 0x03<<4;__enable_irq();
#define CFS_IO_SDA_GET		PBin(9)
#define CFS_IO_SDA				PBout(9)
#define CFS_IO_SCK				PBout(8)

#define CFS_DLY_TIME			1
#define CFS_ADDR					0xDA

#define CFS_REG_SPI_CRTL	0x00
#define CFS_REG_PART_ID		0x01
#define CFS_REG_STATUS		0x02
#define CFS_REG_DATA_MSB	0x06
#define CFS_REG_DATA_CSB	0x07
#define CFS_REG_DATA_LSB	0x08
#define CFS_REG_TEMP_MSB	0x09
#define CFS_REG_TEMP_LSB	0x0A
#define CFS_REG_CMD				0x30
#define CFS_REG_OTP_CMD		0x6C

void cfsInit(void);
int cfsReadData(u32 *pass, u16 *temp);
void cfsZeroCalibration (void);
void cfsSleep(void);
#endif
