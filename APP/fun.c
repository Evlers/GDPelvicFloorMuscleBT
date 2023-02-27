#include "fun.h"
#include "hal.h"
#include "ble_commun.h"
#include "impulse.h"
#include "train.h"
#include "utility.h"
#include "log.h"
#include "ble.h"
#include "cfsensor.h"
#include "booster.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static fun_t fun;
static ble_com_frame_t sendFrame;

static void funTimeCallback(void);
static void voltManage(void);
static void keyManage(void);
static void powerManage(void);
static void ctrlManage(void);
static void uploadManage(void);
static void ledManage(void);

void fun_init(void)
{
	fun.state = 0;
	fun.idleTime = FUN_IDLE_SLEEP_TIME;
	fun.workType = FUN_WORK_TYPE_IDLE;
	fun.batVolt = 3.0;
	fun.lowVoltTime = FUN_LOW_VOLT_TIME;
	fun.lowVoltPormptTime = FUN_LOW_VOLT_TIME;
	fun.sleepTime = FUN_SLEEP_TIME;
	fun.uploadTime = 0;
	fun.chrgTime = FUN_CHRG_TIME;
	HalTimeRegisteringCallbacks(funTimeCallback, 1);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = LED_ALL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FUN_LDO_EN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FUN_LDO_EN_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FUN_CHRG_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FUN_CHRG_PORT, &GPIO_InitStruct);
	FUN_LDO_EN();
	LED_ALL_OFF();
}

void fun_task(void)
{
	ledManage();
	voltManage();
	keyManage();
	ctrlManage();
	powerManage();
	uploadManage();
}

static void ledManage(void)
{
	if((fun.state & FUN_STA_CHRG) || ((fun.state & FUN_STA_POWER_ON) &&(fun.state & FUN_STA_LOW_PROMPT))){
		if(fun.ledTime == 0){
			fun.ledTime = 500;
			if(LED_PORT->ODR & LED_ALL_PIN){
				LED_ALL_ON();
			} else {
				LED_ALL_OFF();
			}
		}
		
	} else if(fun.state & FUN_STA_POWER_ON){
		LED_ALL_ON();
	}	else {
		LED_ALL_OFF();
	}
}

volatile uint32_t pack_count = 0;

static void ctrlManage(void)
{
	ble_com_frame_t *rf = ble_com_recv();
	static uint8_t mode = 0;
	bool restart = 1;
	
	if(!(fun.state & FUN_STA_POWER_ON)){
		if(fun.workType != FUN_WORK_TYPE_IDLE){
			fun.workType = FUN_WORK_TYPE_IDLE;
			impulse_set(0,0,0,0,0,0, true);//关闭电疗
			trainDecompression();//泄压
		}
		return ;
	}
	
	if(fun.workTime == 0 && fun.workType != FUN_WORK_TYPE_IDLE){
		fun.workType = FUN_WORK_TYPE_IDLE;
		impulse_set(0,0,0,0,0,0, true);//关闭电疗
		trainDecompression();//泄压
	}
	
	if(impulse_load_is_null()){
		fun.workType = FUN_WORK_TYPE_NULL_LOAD;
		impulse_set(0,0,0,0,0,0, true);//关闭电疗
	}
	
	if((fun.state & FUN_STA_CHRG) && (fun.state & FUN_STA_POWER_ON))
	{
		fun.state &= ~FUN_STA_POWER_ON;
	}
	
	if(rf != NULL){
		memcpy(&sendFrame, rf, sizeof(ble_com_frame_t));
		HalBuzzSetPeriod(150);
		fun.workType = (fun_work_type)rf->type;
		fun.workTime = rf->timer * 60000;
		if(mode != rf->mode){
			mode = rf->mode;
			restart = 0;
		}
		pack_count ++;
		if(mode == 0)goto close;
		switch(rf->type)
		{
			case FUN_WORK_TYPE_IMPULSE://切换到电疗
//				LOG_D("recv pulse = %u, freq = %u, opt_time = %u, rest_time = %u, speed_inc = %u, emsval = %u", rf->pulse, rf->freq, rf->opt_time, rf->rest_time, rf->speed_inc, rf->emsval);
				impulse_set(rf->pulse, rf->freq, rf->opt_time, rf->rest_time, rf->speed_inc, rf->emsval, restart);//设置电疗参数
			break;
			
			case FUN_WORK_TYPE_TRAIN://切换到训练
				if(impulse_get_intensity() != 0) impulse_set(0,0,0,0,0,0, true);//关闭电疗
				trainInitPressure(rf->press_val);
			break;
			
			default:
				close:
				fun.workType = FUN_WORK_TYPE_IDLE;
			case FUN_WORK_TYPE_IDLE://切换到空闲状态
				if(impulse_get_intensity() != 0) impulse_set(0,0,0,0,0,0, true);//关闭电疗
				trainDecompression();//泄压
			break;
		}
		
	}
}

static void uploadManage(void)
{
	if(fun.uploadTime == 0){
		fun.uploadTime = FUN_UPLOAD_TIME;
		memcpy(sendFrame.uuid, (void *)getUID(), 12);
		for(int i=0;i<4;i++) sendFrame.uuid[12 + i] = sendFrame.uuid[i] ^ sendFrame.uuid[11- i];
		sendFrame.type = fun.workType;
		sendFrame.press_val = trainPressureGet();
		sendFrame.timer = fun.workTime / 60000;
		sendFrame.bat_val =  MIN_VALUE(100, 100 - ((100.0 / (4.25 - FUN_STA_LOW_VOLT)) * (4.10 -  fun.batVolt)));
		ble_com_send(&sendFrame);
	}
}

static void powerManage(void)
{
	if(fun.state & FUN_STA_LOW_VOLT){
		if(fun.state & FUN_STA_POWER_ON)
			fun.state &= ~FUN_STA_POWER_ON;
	}

	if ((fun.state & FUN_STA_POWER_ON) && (fun.workType != FUN_WORK_TYPE_IDLE)) {
		fun.idleTime = FUN_IDLE_SLEEP_TIME;
	}

	if (fun.idleTime == 0 && (fun.state & FUN_STA_POWER_ON)) { // 空闲时间到 电源再开启状态
		fun.state &= ~FUN_STA_POWER_ON; // 关闭电源
		fun.sleepTime = 100; // 100ms后进入休眠
	}
	
	if(fun.state & (FUN_STA_POWER_ON | FUN_STA_CHRG)){
		fun.sleepTime = FUN_SLEEP_TIME;
	}
	
	if(fun.sleepTime == 0)
	{
		if(!(HAL_KEY_PORT & HAL_KEY_SW_ALL)){
			fun.sleepTime =  FUN_SLEEP_TIME;
			return ;
		}

		ble_sleep();

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//开电源管理时钟PWR_Regulator_LowPower
		PWR_WakeUpPinCmd(ENABLE); // 使能WakeUp引脚唤醒
		PWR_EnterSTANDBYMode(); // 进入待机模式(最低功耗)

		NVIC_SystemReset();
		for(int i=0;i<0xFFFFFFFF;i++);
	}
}

static void keyManage(void)
{
	static bool longPress = false;
	if(HalKeyGetLongPress(HAL_KEY_POWER)){
		if(longPress == false){
			longPress = true;
			if(!(fun.state & FUN_STA_POWER_ON) && !(fun.state & FUN_STA_CHRG)){
				FUN_LDO_EN();
				fun.state = FUN_STA_POWER_ON;
				fun.lowVoltPormptTime = fun.lowVoltTime = FUN_LOW_VOLT_TIME;
				fun.idleTime = FUN_IDLE_SLEEP_TIME;
				trainDecompression();
				memset(&sendFrame, 0, sizeof(sendFrame));
			} else {
				fun.state &= ~FUN_STA_POWER_ON;
			}
		}
	} else longPress = false;
}

static void voltManage(void)
{
	static u16 voltAd;
	
	voltAd = (voltAd * 15 + hal.analong.bat_vol) / 16;
	fun.batVolt = BAT_VALUE_VOL(voltAd);
	
	if(!(fun.state & FUN_STA_LOW_VOLT)){
		if(fun.batVolt <= FUN_BAT_LOW_VALUE){
			if(fun.lowVoltTime == 0)
				fun.state |= FUN_STA_LOW_VOLT;
		} else {
			fun.lowVoltTime = FUN_LOW_VOLT_TIME;
		}
	}
	
	if(fun.batVolt <= FUN_BAT_LOW_VALUE + 0.1){
		if(fun.lowVoltPormptTime == 0 && !(fun.state & FUN_STA_LOW_PROMPT))
			fun.state |= FUN_STA_LOW_PROMPT;
	} else {
		fun.lowVoltPormptTime = FUN_LOW_VOLT_TIME;
		if(fun.state & FUN_STA_LOW_PROMPT)fun.state &= ~FUN_STA_LOW_PROMPT;
	}
	
	if(!FUN_CHRG_GET()){
		if(fun.chrgTime == 0 && !(fun.state & FUN_STA_CHRG)){
			fun.state |= FUN_STA_CHRG;
		}
	} else {
		fun.chrgTime = FUN_CHRG_TIME;
		if(fun.state & FUN_STA_CHRG)
			fun.state &= ~FUN_STA_CHRG;
	}
}

static void funTimeCallback(void)
{
	if(fun.ledTime)fun.ledTime --;
	if(fun.lowVoltPormptTime)fun.lowVoltPormptTime --;
	if(fun.lowVoltTime)fun.lowVoltTime --;
	if(fun.uploadTime)fun.uploadTime --;
	if(fun.workTime)fun.workTime --;
	if(fun.sleepTime)fun.sleepTime --;
	if(fun.idleTime)fun.idleTime --;
	if(fun.chrgTime)fun.chrgTime --;
}
