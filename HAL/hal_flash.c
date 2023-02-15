#include "hal_flash.h"
#include "malloc.h"

#if defined (MALLOC) //是否使用动态内存分配 
u8 *FlashPageTmpData;
#else
u8 FlashPageTmpData[FLASH_PAGE_SIZE];
#endif

void HalFlashRead(u32 FlashAddr,u8 *ReadData,u32 ReadLen)
{
	for(u32 i=0;i<ReadLen;i++){
		ReadData[i] = ((u8 *)FlashAddr)[i];
	}
}

HalFlashErrorCode HalFlashWrite(u32 FlashAddr,u8 *WriteData,u16 WriteLen)
{
	volatile u32 secpos = FlashAddr & (~(FLASH_PAGE_SIZE -1 ));//扇区地址 
	volatile u16 secoff = FlashAddr & (FLASH_PAGE_SIZE -1);    //在扇区内的偏移
	#if defined (MALLOC)
		FlashPageTmpData = Malloc(FLASH_PAGE_SIZE);
	#endif
	FLASH_Unlock();
		for(u16 i=0;i<FLASH_PAGE_SIZE;i++)//读出整个扇区
			FlashPageTmpData[i] = ((u8 *)secpos)[i];
		
		if(FLASH_COMPLETE != FLASH_ErasePage(secpos)){//擦除扇区
			FLASH_Lock();
			#if defined (MALLOC)
				Free(FlashPageTmpData);
			#endif
			return HalFlashEraseError;
		}
		
		for(u16 i=0;i<WriteLen;i++){//复制修改扇区数据
			FlashPageTmpData[secoff+i] = WriteData[i];
		}
		
		for(u16 i=0;i<FLASH_PAGE_SIZE/4;i++){//写入整个扇区
			if(FLASH_ProgramWord(secpos+(i*4),((u32 *)FlashPageTmpData)[i]) != FLASH_COMPLETE){
				FLASH_Lock();
				#if defined (MALLOC)
					Free(FlashPageTmpData);
				#endif
				return HalFlashWriteError;
			}
		}
	FLASH_Lock();
	#if defined (MALLOC)
		Free(FlashPageTmpData);
	#endif
	return HalFlashDone;
}

//检测是否为默认出厂设置
HalFlashErrorCode HalFlashDefault(void)
{
	uint32_t defaultValue;
	HalFlashRead(HAL_FLASH_DEFAULT_ADDR,(u8 *)&defaultValue,4);
	return (defaultValue == HAL_FLASH_DEFAULE_VALUE) ? HalFlashDone : HalFlashOtherError;
}

//标记Flash被写过 非出厂设置
HalFlashErrorCode HalFlashFlag(void)
{
	
	if(HalFlashDefault() != HalFlashDone)//有写入过数值
	{
		uint32_t defaultValue = HAL_FLASH_DEFAULE_VALUE;
		return HalFlashWrite(HAL_FLASH_DEFAULT_ADDR,(u8 *)&defaultValue,4);
	}
	return HalFlashDone;
}
