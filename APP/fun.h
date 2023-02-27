#ifndef _FUN_H_
#define _FUN_H_
#include "hal_sys.h"

#define FUN_LDO_EN_PORT			GPIOA
#define FUN_LDO_EN_PIN			GPIO_Pin_11
#define FUN_LDO_EN()				FUN_LDO_EN_PORT->BSRR = FUN_LDO_EN_PIN
#define FUN_LDO_DS()				FUN_LDO_EN_PORT->BRR = FUN_LDO_EN_PIN

#define FUN_CHRG_PORT				GPIOB
#define FUN_CHRG_PIN				GPIO_Pin_15
#define FUN_CHRG_GET()			((FUN_CHRG_PORT->IDR & FUN_CHRG_PIN) ? 1 : 0)

#define LED_PORT						GPIOB
#define LEDA_PIN						GPIO_Pin_3
#define LEDB_PIN						GPIO_Pin_4
#define LEDC_PIN						GPIO_Pin_5
#define LEDD_PIN						GPIO_Pin_6
#define LEDF_PIN						GPIO_Pin_10
#define LED_ALL_PIN					(LEDA_PIN | LEDB_PIN | LEDC_PIN | LEDD_PIN | LEDF_PIN)
#define LED_ALL_ON()				LED_PORT->ODR &= ~LED_ALL_PIN
#define LED_ALL_OFF()				LED_PORT->ODR |= LED_ALL_PIN

#define FUN_LOW_VOLT_TIME		1000	// 持续低电压时间
#define FUN_IDLE_SLEEP_TIME		900000 	// 空闲15分钟休眠
#define FUN_SLEEP_TIME			300000 	// 电源关闭5分钟后休眠
#define FUN_CHRG_TIME			100		// 充电检测时间
#define FUN_UPLOAD_TIME 		100		// 循环上传间隔
#define FUN_BAT_LOW_VALUE		3.1		// 低电压值 单位：伏

#define FUN_STA_LOW_VOLT		0x01
#define FUN_STA_CHRG				0x02
#define FUN_STA_POWER_ON		0x04
#define FUN_STA_LOW_PROMPT	0x08

typedef enum{
	FUN_WORK_TYPE_IDLE			= 2,
	FUN_WORK_TYPE_IMPULSE 	= 3,
	FUN_WORK_TYPE_TRAIN			= 4,
	FUN_WORK_TYPE_NULL_LOAD = 5,
}fun_work_type;

typedef struct{
	u8 	state;
	u16	lowVoltTime;
	u16 lowVoltPormptTime;
	u16 chrgTime;
	fun_work_type workType;
	u32	workTime;
	u16 ledTime;
	u16 uploadTime;
	u32 sleepTime;
	u32 idleTime;
	float batVolt;
}fun_t;

void fun_init(void);
void fun_task(void);
#endif
