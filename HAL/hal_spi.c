#include "hal_spi.h"
#include "delay.h"

void HalSPIInit(SPI_Select SPIx, CPOL_Select Cpol, CPHA_Select Cpha,Baud_Select Baud)
{
  SPI_InitTypeDef  SPI_InitStructure;

	if(SPIx == HAL_SPI_1)
	{
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1ʱ��ʹ�� 	
		
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
		SPI_InitStructure.SPI_CPOL = Cpol;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
		SPI_InitStructure.SPI_CPHA = Cpha;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
		SPI_InitStructure.SPI_BaudRatePrescaler = Baud;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
		SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	 
		SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
		
	}
	else if(SPIx == HAL_SPI_2)
	{
		RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	

		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
		SPI_InitStructure.SPI_CPOL = Cpol;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
		SPI_InitStructure.SPI_CPHA = Cpha;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
		SPI_InitStructure.SPI_BaudRatePrescaler = Baud;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
		SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	 
		SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
		
	}
}
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void HalSPISetSpeed(SPI_Select SPIx,Baud_Select SPI_BaudRatePrescaler)
{
	if(SPIx == HAL_SPI_1)
	{
		assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
		SPI1->CR1&=0XFFC7;
		SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
		SPI_Cmd(SPI1,ENABLE); 
	}
	else if(SPIx == HAL_SPI_2)
	{
		assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
		SPI2->CR1&=0XFFC7;
		SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
		SPI_Cmd(SPI2,ENABLE); 
	}
}

SPI_Error HalSPIWriteByte(SPI_Select SPIx,u8 TxData)
{
	u16 retry = RESET;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}
		SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}
		SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	}
	return HAL_SPI_DONE;
}

SPI_Error HalSPIReadByte(SPI_Select SPIx,u8 *RxData)
{
	u16 retry = RESET;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}	  						    
		*RxData = SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}	  						    
		*RxData = SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
	}
	return HAL_SPI_DONE;
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 HalSPIReadWriteByte(SPI_Select SPIx,u8 TxData)
{
	u16 retry;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
			retry++;
			if(retry>2000)return 0;
		}
		SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
		retry=0;

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
			retry++;
			if(retry>2000)return 0;
		}	  						    
		return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
			retry++;
			if(retry>2000)return 0;
		}
		SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
		retry=0;

		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
			retry++;
			if(retry>2000)return 0;
		}	  						    
		return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
	}		
	return 0;
}
//���ط��͵����ݳ���
u8 HalSPIWrite(SPI_Select SPIx,u8 *TxData,u8 Len)
{
	for(u8 i=0;i<Len;i++)
	{
		if(HalSPIWriteByte(SPIx,TxData[i]) == HAL_SPI_ERROR)
			return i+1;
	}
	return Len;
}
