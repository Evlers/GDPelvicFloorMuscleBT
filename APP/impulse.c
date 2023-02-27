#include "impulse.h"
#include "impulse_list.h"
#include "hal.h"
#include "booster.h"

#include <stdio.h>
#include <string.h>

//盆底肌修复(电疗)

impulse_t impulse;

void impulse_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	IMPULSE_PWM_CLOSE();
	
	GPIO_InitStruct.GPIO_Pin = IMPULSE_PWM_P_PIN | IMPULSE_PWM_N_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IMPULSE_PWM_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = IMPULSE_OP_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IMPULSE_OP_PORT, &GPIO_InitStruct);

	//测试IO
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	IMPULSE_PWM_CLOSE();
	
	impulse.nullLoadCnt = 0;
	
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	NVIC_InitTypeDef NVICInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIMInitstruct.TIM_Prescaler = 72;//分频
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIMInitstruct.TIM_Period = 10-1;//重载值
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM2,&TIMInitstruct);//初始化定时器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能定时器更新中断
	TIM_Cmd(TIM2,ENABLE);//使能定时器
	
	NVICInitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0x00;//中断优先级
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断
	NVIC_Init(&NVICInitStruct);//初始化中断向量表
	
	//impulse_set(200, 2, 1, 0, 3000, 50, true);
}

//iw：脉冲宽度 	50us~450us
//freq：频率		2~100Hz
//rt：运行时间	2~99s
//st：休息时间	2~99s
//qt：加速时间	300~9900ms
//in：强度			0~100
void impulse_set(u16 iw, u8 freq, u8 rt, u8 st, u16 qt, u8 in, bool restart)
{
	if(freq < 2)freq = 0;
	if(freq > 100)freq = 100;
	if(iw < 50)iw = 50;
	if(iw > 450)iw = 450;
	if(rt < 1)rt = 1;
	if(rt > 99)rt = 99;
	if(st > 99)st = 99;
	if(qt < 300)qt = 30;
	if(qt > 9900)qt = 9900;
	if(in > 100)in = 100;
	
	__disable_irq();
	impulse.list.intensity = 0;
	impulse.nullLoadCnt = 0;
	__enable_irq();
	
	if(restart) memset(&impulse.list, 0, sizeof(impulse.list));
	impulse.list.impulseWidth = iw / 10;//脉宽
	impulse.list.frequency = 1000 / freq;//频率
	impulse.list.runTime = rt * 1000;//运行时间
	impulse.list.stopTime = st * 1000;//休息时间
	impulse.list.quickenTime = qt;//加速时间
	impulse.list.intensity = in;//强度
}

u8 impulse_get_intensity(void)
{
	return impulse.list.intensity;
}

bool impulse_load_is_null(void)
{
	return (impulse.nullLoadCnt >= 20) ? true : false;
}

static void loadCheck(void)
{
	if(!IMPULSE_OP_GET()){
		if(impulse.nullLoadCnt < 0xFF)
			impulse.nullLoadCnt ++;
	} else {
		impulse.nullLoadCnt = 0;
	}
}

//电压调制
static void voltModulate(void)
{
	if(impulse.list.frequency > 66/*15Hz*/){//小于15hz的情况下强度与输出电压挂钩
		booster_volt_set(_IQ15(1) * impulse.list.intensity / IMPULSE_INTENSITY_MAX / (_IQ15(1) / IMPULSE_VOLT_MAX));//计算输出电压 IMPULSE_VOLT_MAX / IMPULSE_INTENSITY_MAX * impulse.list.intensity
		
	} else {//大于15Hz的情况下进行启动电压的调节
		if(impulse.list.intensity <= 10){//小于10的强度进行输出电压的调整
			uint8_t startVolt = _IQ15(1) * impulse.list.intensity / IMPULSE_INTENSITY_MAX / (_IQ15(1) / IMPULSE_VOLT_MAX);//计算启动电压 IMPULSE_VOLT_MAX / IMPULSE_INTENSITY_MAX * impulse.list.intensity
			uint8_t finalVolt = (_IQ15(1) / 10 * impulse.list.intensity) / (_IQ15(1) / (IMPULSE_VOLT_MAX - startVolt));//计算最终输出值 IMPULSE_VOLT_MAX / 10 * impulse.list.intensity)
			uint8_t shadow = _IQ15(1) * impulse.list.qCnt / impulse.list.quickenTime / (_IQ15(1) / finalVolt);//计算渐变电压  finalVolt / impulse.list.quickenTime * impulse.list.qCnt
			booster_volt_set(startVolt + shadow);//设置输出电压 
			
		} else {//大于10的强度进行启动电压的调节
			uint8_t startVolt = _IQ15(1) * impulse.list.intensity / IMPULSE_INTENSITY_MAX / (_IQ15(1) / IMPULSE_VOLT_MAX);//计算启动电压 IMPULSE_VOLT_MAX / IMPULSE_INTENSITY_MAX * impulse.list.intensity
			uint8_t shadow = _IQ15(1) * impulse.list.qCnt / impulse.list.quickenTime / (_IQ15(1) / (IMPULSE_VOLT_MAX - startVolt));//计算渐变电压  (IMPULSE_VOLT_MAX - startVolt) / impulse.list.quickenTime * impulse.list.qCnt
			booster_volt_set(startVolt + shadow);//设置输出电压
		}
	}
}

//输出调制
static void impulseModulate(void)
{
	//加速时间以及运行时间调制
	if(	(impulse.list.rCnt == 0 && impulse.list.qCnt < impulse.list.quickenTime) || //运行时间没开始时加速时间不到
			(impulse.list.rCnt >= impulse.list.runTime && impulse.list.qCnt > 0) || 		//运行时间结束时加速时间还没结束
			 impulse.list.rCnt < impulse.list.runTime)																	//运行时间开始
	{
		//频率调制
		if(impulse.list.fCnt == 0)
		{
			//脉冲宽度调制
			if(impulse.list.wCnt == 0){
				IMPULSE_PWM_N_CLR();
				IMPULSE_PWM_P_SET();
			} else if(impulse.list.wCnt == (impulse.list.impulseWidth / 2)){
				loadCheck();
				IMPULSE_PWM_P_CLR();
				IMPULSE_PWM_N_SET();
			}
			
			if(impulse.list.wCnt >= impulse.list.impulseWidth){
				IMPULSE_PWM_CLOSE();
			}else impulse.list.wCnt ++;
			
		}
	}
	
	impulse.list.ms++;
	if(impulse.list.ms >= 100){
		impulse.list.ms = 0;
		
		//1ms***************************************
		
		if(impulse.list.rCnt == 0 && impulse.list.qCnt < impulse.list.quickenTime) impulse.list.qCnt ++;//运行时间还没开始且加速时间不到时累加加速时间计数
		if(impulse.list.rCnt >= impulse.list.runTime && impulse.list.qCnt > 0) impulse.list.qCnt --;//运行时间完成且加速时间未减到零进行递减计数
		
		//频率累加计数
		impulse.list.fCnt ++;
		if(impulse.list.fCnt >= impulse.list.frequency){
			impulse.list.fCnt = 0;
			impulse.list.wCnt = 0;
		}
		
		if(impulse.list.qCnt >= impulse.list.quickenTime && impulse.list.rCnt < impulse.list.runTime)impulse.list.rCnt ++;//开始的加速时间到且运行时间没完成 累加运行时间计数
		else if(impulse.list.qCnt == 0 && impulse.list.sCnt < impulse.list.stopTime)impulse.list.sCnt ++;//结束的加速时间完成且运行时间完成且停止时间不到时累加停止时间计数
		
		if(impulse.list.sCnt >= impulse.list.stopTime){//停止时间完成时
			impulse.list.sCnt = impulse.list.rCnt = 0;
		}
		
		//*******************************************
	}
}

//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		if(impulse.list.intensity)
		{
			//GPIOF->BSRR = GPIO_Pin_12;
			
			voltModulate();
			impulseModulate();
			
			//GPIOF->BRR = GPIO_Pin_12;
			
		} else {
			booster_volt_set(0);
			IMPULSE_PWM_CLOSE();
		}
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}
