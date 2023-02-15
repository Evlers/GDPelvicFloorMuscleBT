#include "hal_adc.h"

#define ADC_SampleTime	ADC_SampleTime_55Cycles5

static void ADCChannelGPIOInit(u8 Channel);
static u16 *ADCChannelBuff;
static void (*ConvertDoneCallback)(void);

static void NVICInit(void)
{
	NVIC_InitTypeDef NVICInitStruct;
	NVICInitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVICInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICInitStruct);
}

void HalAdc2Init(u8 *ChannelTable, int ChannelNum)
{
	ADC_InitTypeDef ADCInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//��λ����

	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;//�����ڶ���ģʽ
	ADCInitStruct.ADC_ScanConvMode = DISABLE;//��ͨ��ɨ��
	ADCInitStruct.ADC_ContinuousConvMode = DISABLE;//�ر�����ɨ��
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�����ⲿ����
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;//���Ҷ���
	ADCInitStruct.ADC_NbrOfChannel = 1 ;//�ɼ�ͨ����
	ADC_Init(ADC2,&ADCInitStruct);//��ʼ��ADC
	
	for(int i = 0; i < ChannelNum; i++)
		ADCChannelGPIOInit(ChannelTable[i]);
	
	ADC_Cmd(ADC2,ENABLE); //ʹ��ADC
	
	ADC_ResetCalibration(ADC2);//��λУ׼
	while(ADC_GetResetCalibrationStatus(ADC2) != RESET);//�ȴ���λУ׼���
	ADC_StartCalibration(ADC2);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC2) != RESET);//�ȴ�У׼���
	//ADC_SoftwareStartConvCmd(ADC2, ENABLE);	//��ʼת��
}

void HalAdcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum)
{
	ADC_InitTypeDef ADCInitStruct;
	DMA_InitTypeDef DMA_InitStructure;
	ADCChannelBuff = ChannelBuff;
	ConvertDoneCallback = 0;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
	
	DMA_DeInit(DMA1_Channel1);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//ADC��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ChannelBuff; //�ڴ��ַ  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //����(�����赽�ڴ�)  
	DMA_InitStructure.DMA_BufferSize = ChannelNum; //�������ݵĴ�С  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�̶�  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�̶�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //�������ݵ�λ  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //�ڴ����ݵ�λ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; //DMAģʽ��ѭ������  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ; //���ȼ�����  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   //��ֹ�ڴ浽�ڴ�Ĵ���  
		
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA1��4ͨ��  
	DMA_ITConfig (DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1,ENABLE);  
	NVICInit();
	
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;//�����ڶ���ģʽ
	ADCInitStruct.ADC_ScanConvMode = ENABLE;//��ͨ��ɨ��
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;//�ر�����ɨ��
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�����ⲿ����
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;//���Ҷ���
	ADCInitStruct.ADC_NbrOfChannel = ChannelNum ;//�ɼ�ͨ����
	ADC_Init(ADC1,&ADCInitStruct);//��ʼ��ADC1
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); //ADCʱ�ӷ�Ƶ 72/8 = 9M
	
	for(int i=0;i<ChannelNum;i++){
		ADC_RegularChannelConfig(ADC1, ChannelTable[i], i + 1, ADC_SampleTime);
		ADCChannelGPIOInit(ChannelTable[i]);//��ʼ��IO
	}
	
	ADC_DMACmd(ADC1,ENABLE);  
  ADC_Cmd(ADC1,ENABLE); //ʹ��ADC1
	
	ADC_ResetCalibration(ADC1);//��λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1) != RESET);//�ȴ���λУ׼���
	ADC_StartCalibration(ADC1);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC1) != RESET);//�ȴ�У׼���
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//��ʼת��
}

void DMA1_Channel1_IRQHandler(void)
{
	if(ConvertDoneCallback != 0)
		ConvertDoneCallback();
	DMA_ClearFlag (DMA1_FLAG_TC1);
}

void HalAdcSetConvertCallback(void (*func)(void))
{
	ConvertDoneCallback = func;
}

u16 HalAdc2Read(u8 channel)
{
	ADC_RegularChannelConfig(ADC2, channel, 1, ADC_SampleTime_239Cycles5); 
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
	ADC_SoftwareStartConvCmd(ADC2, DISABLE);
	return ADC_GetConversionValue(ADC2);
}

u16 HalAdcRead(u8 ADC_Channel)
{
	ADCChannelBuff[ADC_Channel] = 0x8000;
	while(ADCChannelBuff[ADC_Channel] & 0x8000);		//�ȴ�ת�����
	return ADCChannelBuff[ADC_Channel];
}

void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len)
{
	u16 i;
	for(i=0;i<Len;i++)
		Fifo[i] = HalAdcRead(ADC_Channel);
}

static void ADCChannelGPIOInit(u8 Channel)
{
	GPIO_InitTypeDef GPIOInitStruct;
	GPIO_TypeDef *GPIOx;
	switch(Channel)
	{
		case 0:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOA;break;
		case 1:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOA;break;
		case 2:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIOx = GPIOA;break;
		case 3:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIOx = GPIOA;break;
		case 4:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_4;
			GPIOx = GPIOA;break;
		case 5:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
			GPIOx = GPIOA;break;
		case 6:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_6;
			GPIOx = GPIOA;break;
		case 7:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_7;
			GPIOx = GPIOA;break;
		case 8:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOB;break;
		case 9:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOB;break;
		case 10:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOC;break;
		case 11:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOC;break;
		case 12:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIOx = GPIOC;break;
		case 13:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIOx = GPIOC;break;
		case 14:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_4;
			GPIOx = GPIOC;break;
		case 15:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
			GPIOx = GPIOC;break;
		
	}
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx,&GPIOInitStruct);
}
