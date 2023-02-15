#ifndef _STMFLASH_H_
#define _STMFLASH_H_
#include "stm32f10x.h"

#define	DESTRUCT_SAVE_ADDR			0x0800F800//�Իٴ����¼��ַ 128Byte
#define ENCRYPTION_SAVE_ADDR		0x0800F880//����ID����λ�� 32Byte
#define FLASH_START_ADDR				0x0800F8A0//�û��Զ������� 1888Byte

#define HAL_FLASH_DEFAULE_VALUE	0xAA55AA55

#define HAL_FLASH_DEFAULT_ADDR	FLASH_START_ADDR	//4Byte
#define HAL_FLASH_ZERO_ADDR			HAL_FLASH_DEFAULT_ADDR + 4//4Byte


#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

typedef enum{
	HalFlashDone = 0,	//��ɲ���
	HalFlashWriteError,//д�����
	HalFlashEraseError,//��������
	HalFlashOtherError,//��������
}HalFlashErrorCode;


void HalFlashRead(u32 FlashAddr,u8 *ReadData,u32 ReadLen);
HalFlashErrorCode HalFlashWrite(u32 FlashAddr,u8 *WriteData,u16 WriteLen);

HalFlashErrorCode HalFlashDefault(void);
HalFlashErrorCode HalFlashFlag(void);
#endif
