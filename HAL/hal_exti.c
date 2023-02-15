#include "hal_exti.h"

HalExti_t HalExti[HAL_EXTI_MAX_CB_NUM];

static void gpioConfig(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode);

//�ⲿ�жϳ�ʼ��
//����GPIOx:����HalGpioTypeDefö�ٶ��� 
//����PINx:0~15
//����IOMode:�ο�EXTITrigger_TypeDef�ඨ�� 
//����Trigger:�ο�EXTITrigger_TypeDef (ѡ��������)
//����CB:�жϻص�����
void HalExtiInit(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode,EXTITrigger_TypeDef Trigger,HalExtiCallBack_t CB)
{
	u8 i;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	gpioConfig(GPIOx,PINx,IOMode);
	
	GPIO_EXTILineConfig(GPIOx,PINx);
	EXTI_InitStructure.EXTI_Line = BV(PINx);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = Trigger;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	if(PINx <= 4)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn+PINx;
	else if(PINx <= 9)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	else 
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	for(i=0;i<HAL_EXTI_MAX_CB_NUM;i++){//Ѱ�ҿջ��岢ע��
		if(HalExti[i].cb == 0){
			HalExti[i].line = PINx;
			HalExti[i].cb = CB;
			break;
		}
	}
}

static void gpioConfig(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|(RCC_APB2Periph_GPIOA<<GPIOx),ENABLE);
	GPIO_InitStructure.GPIO_Pin = BV(PINx);
	GPIO_InitStructure.GPIO_Mode = IOMode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)(GPIOA_BASE+(GPIOx*0x400)),&GPIO_InitStructure);
}

static void HalExtiInterrupt(u8 line)
{
	for(u8 i=0;i<HAL_EXTI_MAX_CB_NUM;i++){//Ѱ����Ҫ�ص��ĺ���
		if(HalExti[i].cb){//�����ע��ص�
			if(HalExti[i].line == line)//����Ǵ�IO���жϻص�
				HalExti[i].cb();//����
		}
	}
}

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line0))
	{
		HalExtiInterrupt(0);
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}

//�ⲿ�ж�1������� 
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line1))
	{
		HalExtiInterrupt(1);
	}
	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE1�ϵ��жϱ�־λ  
}

//�ⲿ�ж�2������� 
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line2))
	{
		HalExtiInterrupt(2);
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //���LINE3�ϵ��жϱ�־λ  
}

//�ⲿ�ж�3������� 
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line3))
	{
		HalExtiInterrupt(3);
	}
	EXTI_ClearITPendingBit(EXTI_Line3); //���LINE3�ϵ��жϱ�־λ  
}

//�ⲿ�ж�4������� 
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line4))
	{
		HalExtiInterrupt(4);
	}
	EXTI_ClearITPendingBit(EXTI_Line4); //���LINE4�ϵ��жϱ�־λ  
}

//�ⲿ�ж�5-9������� 
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5))
	{
		HalExtiInterrupt(5);
		EXTI_ClearITPendingBit(EXTI_Line5); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line6))
	{
		HalExtiInterrupt(6);
		EXTI_ClearITPendingBit(EXTI_Line6); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line7))
	{
		HalExtiInterrupt(7);
		EXTI_ClearITPendingBit(EXTI_Line7); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line8))
	{
		HalExtiInterrupt(8);
		EXTI_ClearITPendingBit(EXTI_Line8); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line9))
	{
		HalExtiInterrupt(9);
		EXTI_ClearITPendingBit(EXTI_Line9); //���
	}
}

//�ⲿ�ж�10-15������� 
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line10))
	{
		HalExtiInterrupt(10);
		EXTI_ClearITPendingBit(EXTI_Line10); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line11))
	{
		HalExtiInterrupt(11);
		EXTI_ClearITPendingBit(EXTI_Line11); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line12))
	{
		HalExtiInterrupt(12);
		EXTI_ClearITPendingBit(EXTI_Line12); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line13))
	{
		HalExtiInterrupt(13);
		EXTI_ClearITPendingBit(EXTI_Line13); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line14))
	{
		HalExtiInterrupt(14);
		EXTI_ClearITPendingBit(EXTI_Line14); //���
	}
	if(EXTI_GetFlagStatus(EXTI_Line15))
	{
		HalExtiInterrupt(15);
		EXTI_ClearITPendingBit(EXTI_Line15); //���
	}
}
