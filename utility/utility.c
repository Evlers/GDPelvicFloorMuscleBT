#include "utility.h"



u8 *getUID(void) 
{
	static u8 uid[STM32_UNIQUE_ID_SIZE] = {0};
	vu8* addr = (vu8*)(0x1FFFF7E8);   //UIDÊ×µØÖ·
	for(u8 i = 0; i < STM32_UNIQUE_ID_SIZE; ++i)
		uid[i] = addr[i];
	return uid;
}

