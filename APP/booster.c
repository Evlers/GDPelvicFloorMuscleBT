#include "booster.h"
#include "hal.h"
#include "log.h"
#include "delay.h"
#include <string.h>

//电疗升压管理

booster_t booster;

static void booster_TimerCallback(void);


void booster_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = BOOSTER_AD_EN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BOOSTER_AD_EN_PORT, &GPIO_InitStruct);
	
	BOOSTER_AD_EN();
	//BOOSTER_DISC_EN();
	
	booster.volt = 0;
	
	hal_TIM1_OC_config(BOOSTER_PREIOD);
	HalTimeRegisteringCallbacks(booster_TimerCallback, 1);
	
	memset(&booster.pid, 0, sizeof(booster.pid));
	
	//booster.pid.kd = _IQ15(0.01);
	booster.pid.ki = _IQ15(0.01);
	booster.pid.kp = _IQ15(0.9);
}

void booster_task(void)
{
	
	//booster_TimerCallback();
}

void booster_volt_set(u8 volt)
{
	booster.volt = volt;
}

#define sumvalue    7

#define FUNC_PID_SINGLE_INTEGRAL_MAX _IQ15(0.01)

q15 PidComputeQ15(q15 err, pid_q15_type *pid)
{
	s32 tmp; 
	//integral
	tmp = ((s32)err * pid->ki) >> 15;
	tmp = tmp > FUNC_PID_SINGLE_INTEGRAL_MAX ? FUNC_PID_SINGLE_INTEGRAL_MAX+pid->f : tmp+pid->f;
	
	if(tmp > _IQ15(1.0))
		pid->f = _IQ15(1.0);
	else if(tmp < _IQ15(-1.0))
		pid->f = _IQ15(-1.0);
	else
		pid->f = tmp;
	
	tmp = pid->f;
	tmp += ((s32)err * pid->kp) >> 15; //scale
	//tmp += (s32)(err - pid->errl) * pid->kd / IQ15(1.0);  //differential
	pid->errl = err;

	if(tmp > _IQ15(1.0)) tmp = _IQ15(1.0);
	else if(tmp < 0) tmp = 0;
	
	return (q15)tmp;
}

volatile float booster_volt;

static void booster_TimerCallback(void)
{
	static u32 value = 0;
	u32 volt, curDuty = hal_TIM1_OC1_duty_get();
	
	value = hal.analong.out_vol;//((value * 7) + hal.analong.out_vol) / 8;//低通滤波处理
	
	volt = 3300 * (_IQ15(1) * value / 4096) / _IQ15(1);//计算IO电压 3.3 / 4096 * volt 单位mv
	volt = ((_IQ15(1) * volt)) / (_IQ15(1) * BOOSTER_V_BUS_R2 / (BOOSTER_V_BUS_R1 + BOOSTER_V_BUS_R2));//计算输出电压 io / BOOSTER_V_BUS_R2 * (BOOSTER_V_BUS_R1 + BOOSTER_V_BUS_R2)
	volt /= 1000;
	
	booster_volt = volt;
	
	q15 err = ((long)booster.volt - (long)volt) * _IQ15(1.0) / 50;
	uint32_t duty = BOOSTER_PREIOD * PidComputeQ15(err, &booster.pid) / _IQ15(1.0);
	hal_TIM1_OC1_duty_set(duty);
	
	if( volt >= booster.volt && (volt - booster.volt) > 10 ){
		BOOSTER_DISC_EN();
	} else {
		BOOSTER_DISC_DS();
	}
}
