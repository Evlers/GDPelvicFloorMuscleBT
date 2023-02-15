#include "hal.h"

const u8 channelTable[] = {4, 9};
const u8 adc2channelTable[] = {0};

hal_typedef hal;

void hal_initialzation(void)
{
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	HalAdcInit((u8 *)channelTable, (u16 *)&hal.analong, sizeof(channelTable));
	HalAdc2Init((u8 *)adc2channelTable, sizeof(adc2channelTable));
}


void hal_TIM1_OC_config(u16 period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = period - 1;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;          //使能输出
	TIM_OCInitStruct.TIM_Pulse = 0;                                     //占空比0%
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);                               

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//GPIO_PinRemapConfig(GPIO_Remap_TIM13, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void hal_TIM1_OC1_duty_set(u16 duty)
{
  TIM1->CCR1 = duty;
}

u16 hal_TIM1_OC1_duty_get(void)
{
	return TIM1->CCR1;
}


void hal_TIM3_OC_config(u16 period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = period - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;          //使能输出
	TIM_OCInitStruct.TIM_Pulse = 0;                                     //占空比0%
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);                               

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//GPIO_PinRemapConfig(GPIO_Remap_TIM13, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void hal_TIM3_OC1_duty_set(u16 duty)
{
  TIM3->CCR1 = duty;
}

u16 hal_TIM3_OC1_duty_get(void)
{
	return TIM3->CCR1;
}


void EXTI15_10_IRQHandler(void)
{
	EXTI_ClearFlag(EXTI_Line12);
	EXTI_ClearFlag(EXTI_Line15);
}

#define PWR_SLEEPEntry_WFI         ((uint8_t)0x01)
#define PWR_SLEEPEntry_WFE         ((uint8_t)0x02)
#define SysCtrl_SLEEPONEXIT_Set    ((u16)0x0002)
#define SysCtrl_SLEEPDEEP_Set  		 ((u16)0x0004)

static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);

#ifndef STM32F10X_CL
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
#else
    if (HSE_VALUE <= 24000000)
	{
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
	}
	else
	{
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;
	}
#endif /* STM32F10X_CL */
#endif
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
    /* Select HSE as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;    

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }  
}

static void HalMinSystemClock(void)
{
	/* Reset the RCC clock configuration to the default reset state(for debug purpose) */
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#ifndef STM32F10X_CL
  RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
  RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */   
  
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_CL
  /* Reset PLL2ON and PLL3ON bits */
  RCC->CR &= (uint32_t)0xEBFFFFFF;

  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x00FF0000;

  /* Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;

  /* Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;      
#else
  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */
    
#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
  #ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl(); 
  #endif /* DATA_IN_ExtSRAM */
#endif 

	SetSysClockToHSE();
}
#include "fun.h"
#include "ble.h"

void HalSleep(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = FUN_LDO_EN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FUN_LDO_EN_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FUN_CHRG_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FUN_CHRG_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = HAL_KEY_SW_ALL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = BLE_WAKE_PIN | BLE_RESET_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLE_GPIO_PORT, &GPIO_InitStruct);
	
	BLE_RESET_CLR;
	BLE_WAKE_SET;
	
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);//GPIOB12
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);//GPIOB15
	
	// EXTI_InitTypeDef EXTI_InitStructure;
	// NVIC_InitTypeDef NVIC_InitStructure;
	
	// EXTI_DeInit();
	
	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        //外部按键触发中断
	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;    //下降沿触发
	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;                  //外部中断使能
	// EXTI_InitStructure.EXTI_Line = EXTI_Line12;             	//外部中断线号
	// EXTI_Init(&EXTI_InitStructure);
	
	// EXTI_InitStructure.EXTI_Line = EXTI_Line15;             	//外部中断线号
	// EXTI_Init(&EXTI_InitStructure);
	
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;                  
	// NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//开电源管理时钟PWR_Regulator_LowPower
	if(!(HAL_KEY_PORT & HAL_KEY_SW_ALL)){
		return ;
	}
	HalMinSystemClock();
	PWR_WakeUpPinCmd(ENABLE); // 使能WakeUp引脚唤醒
	PWR_EnterSTANDBYMode(); // 进入待机模式(最低功耗)
	__WFI();
}

//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
void IWDG_Init(u8 prer,u16 rlr) 
{
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作，即取消写保护
	IWDG_SetPrescaler(prer);  //设置IWDG预分频值
	IWDG_SetReload(rlr);  //设置IWDG重装载值
	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
	IWDG_Enable();  //使能IWDG
}

//喂独立看门狗
void IWDG_Feed(void)
{   
 	IWDG_ReloadCounter();//reload										   
}

