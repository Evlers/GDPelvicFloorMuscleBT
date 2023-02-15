#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_
#include "stm32f10x.h"

typedef enum{
HAL_SPI_1,
HAL_SPI_2,
}SPI_Select;//ѡ������

typedef enum{
HAL_SPI_CPOL_LOW = SPI_CPOL_Low,
HAL_SPI_CPOL_HIGH = SPI_CPOL_High,
}CPOL_Select;//ѡ�����ʱ�ӵĸߵ�

typedef enum{
HAL_SPI_CPHA_1EDGE = SPI_CPHA_1Edge,
HAL_SPI_CPHA_2EDGE = SPI_CPHA_2Edge,
}CPHA_Select;//ѡ��ڼ��������ض�ȡ��������

typedef enum{
HAL_SPI_BAUD_2 		= SPI_BaudRatePrescaler_2,
HAL_SPI_BAUD_4 		= SPI_BaudRatePrescaler_4,
HAL_SPI_BAUD_8		= SPI_BaudRatePrescaler_8,
HAL_SPI_BAUD_16 	= SPI_BaudRatePrescaler_16,
HAL_SPI_BAUD_32 	= SPI_BaudRatePrescaler_32,
HAL_SPI_BAUD_64 	= SPI_BaudRatePrescaler_64,
HAL_SPI_BAUD_128 	= SPI_BaudRatePrescaler_128,
HAL_SPI_BAUD_256 	= SPI_BaudRatePrescaler_256,
}Baud_Select;//ѡ������ͨѶ����

typedef enum{
HAL_SPI_ERROR,
HAL_SPI_DONE,
}SPI_Error;//SPI���󱨸�

void HalSPIInit(SPI_Select SPIx, CPOL_Select Cpol, CPHA_Select Cpha,Baud_Select Baud);
void HalSPISetSpeed(SPI_Select SPIx,Baud_Select SPI_BaudRatePrescaler);
SPI_Error HalSPIWriteByte(SPI_Select SPIx,u8 TxData);
SPI_Error HalSPIReadByte(SPI_Select SPIx,u8 *RxData);
u8 HalSPIReadWriteByte(SPI_Select SPIx,u8 TxData);
u8 HalSPIWrite(SPI_Select SPIx,u8 *TxData,u8 Len);
#endif
