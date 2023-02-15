#ifndef _STMFLASH_H_
#define _STMFLASH_H_
#include "stm32f10x.h"

#define	DESTRUCT_SAVE_ADDR			0x0800F800//自毁代码记录地址 128Byte
#define ENCRYPTION_SAVE_ADDR		0x0800F880//加密ID储存位置 32Byte
#define FLASH_START_ADDR				0x0800F8A0//用户自定义区域 1888Byte

#define HAL_FLASH_DEFAULE_VALUE	0xAA55AA55

#define HAL_FLASH_DEFAULT_ADDR	FLASH_START_ADDR	//4Byte
#define HAL_FLASH_ZERO_ADDR			HAL_FLASH_DEFAULT_ADDR + 4//4Byte


#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

typedef enum{
	HalFlashDone = 0,	//完成操作
	HalFlashWriteError,//写入错误
	HalFlashEraseError,//擦除错误
	HalFlashOtherError,//其他错误
}HalFlashErrorCode;


void HalFlashRead(u32 FlashAddr,u8 *ReadData,u32 ReadLen);
HalFlashErrorCode HalFlashWrite(u32 FlashAddr,u8 *WriteData,u16 WriteLen);

HalFlashErrorCode HalFlashDefault(void);
HalFlashErrorCode HalFlashFlag(void);
#endif
