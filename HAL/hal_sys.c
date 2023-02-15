#include "hal_sys.h"

/*
(1)ʹ��ʱ��main()������Ӧд�ɣ�Stm32_Clock_Init(12)����8/2*12=48MHz.

(2)Ϊʹdelay_ms()�Ⱥ���ʱ��׼ȷ��Ӧ��system_stm32f10x.c�н�

(3)#define SYSCLK_FREQ_48MHz  48000000�������

(4)ͬʱҪע����USBʱ�ӳ�ʼ�������в��ٶ�PLLʱ�ӷ�Ƶ����ʱ�����üĴ���(RCC_CFGR)��22λӦ����Ϊ1

(5)����ڲ�ʱ��Ҫ����ȷʹ�ã���Ҫ������.s�ļ�������sysinit
*/

void HalSysClockInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
}


//�����ж�������
void HalSysSetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
/*
__asm void SystemReset(void)
{
	MOV R0, #1 //;
	MSR FAULTMASK, R0 //; ??FAULTMASK ????????
	LDR R0, =0xE000ED0C //;
	LDR R1, =0x05FA0004 //;
	STR R1, [R0] //; ??????
	deadloop
	B deadloop //; ???????????????
}*/
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
