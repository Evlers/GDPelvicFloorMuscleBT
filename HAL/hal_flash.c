#include "hal_flash.h"
#include "malloc.h"

#if defined (MALLOC) //�Ƿ�ʹ�ö�̬�ڴ���� 
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
	volatile u32 secpos = FlashAddr & (~(FLASH_PAGE_SIZE -1 ));//������ַ 
	volatile u16 secoff = FlashAddr & (FLASH_PAGE_SIZE -1);    //�������ڵ�ƫ��
	#if defined (MALLOC)
		FlashPageTmpData = Malloc(FLASH_PAGE_SIZE);
	#endif
	FLASH_Unlock();
		for(u16 i=0;i<FLASH_PAGE_SIZE;i++)//������������
			FlashPageTmpData[i] = ((u8 *)secpos)[i];
		
		if(FLASH_COMPLETE != FLASH_ErasePage(secpos)){//��������
			FLASH_Lock();
			#if defined (MALLOC)
				Free(FlashPageTmpData);
			#endif
			return HalFlashEraseError;
		}
		
		for(u16 i=0;i<WriteLen;i++){//�����޸���������
			FlashPageTmpData[secoff+i] = WriteData[i];
		}
		
		for(u16 i=0;i<FLASH_PAGE_SIZE/4;i++){//д����������
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

//����Ƿ�ΪĬ�ϳ�������
HalFlashErrorCode HalFlashDefault(void)
{
	uint32_t defaultValue;
	HalFlashRead(HAL_FLASH_DEFAULT_ADDR,(u8 *)&defaultValue,4);
	return (defaultValue == HAL_FLASH_DEFAULE_VALUE) ? HalFlashDone : HalFlashOtherError;
}

//���Flash��д�� �ǳ�������
HalFlashErrorCode HalFlashFlag(void)
{
	
	if(HalFlashDefault() != HalFlashDone)//��д�����ֵ
	{
		uint32_t defaultValue = HAL_FLASH_DEFAULE_VALUE;
		return HalFlashWrite(HAL_FLASH_DEFAULT_ADDR,(u8 *)&defaultValue,4);
	}
	return HalFlashDone;
}
