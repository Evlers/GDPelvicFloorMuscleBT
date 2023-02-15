#include "hal_sys.h"

/*
(1)使用时在main()函数中应写成：Stm32_Clock_Init(12)，即8/2*12=48MHz.

(2)为使delay_ms()等函数时钟准确，应在system_stm32f10x.c中将

(3)#define SYSCLK_FREQ_48MHz  48000000解除屏蔽

(4)同时要注意在USB时钟初始化函数中不再对PLL时钟分频。即时钟配置寄存器(RCC_CFGR)第22位应设置为1

(5)最后，内部时钟要想正确使用，需要在启动.s文件中屏蔽sysinit
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


//配置中断向量表
void HalSysSetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
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
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
