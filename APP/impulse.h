#ifndef _IMPULSE_H_
#define _IMPULSE_H_
#include "hal_sys.h"
#include <stdbool.h>

#define IMPULSE_PWM_PORT			GPIOA
#define IMPULSE_PWM_P_PIN			GPIO_Pin_9
#define IMPULSE_PWM_N_PIN			GPIO_Pin_10
#define IMPULSE_OP_PORT				GPIOB
#define IMPULSE_OP_PIN				GPIO_Pin_7

#define IMPULSE_PWM_P_SET()		IMPULSE_PWM_PORT->BSRR = IMPULSE_PWM_P_PIN
#define IMPULSE_PWM_P_CLR()		IMPULSE_PWM_PORT->BRR = IMPULSE_PWM_P_PIN

#define IMPULSE_PWM_N_SET()		IMPULSE_PWM_PORT->BSRR = IMPULSE_PWM_N_PIN
#define IMPULSE_PWM_N_CLR()		IMPULSE_PWM_PORT->BRR = IMPULSE_PWM_N_PIN

#define IMPULSE_PWM_CLOSE()		IMPULSE_PWM_P_CLR();IMPULSE_PWM_N_CLR();

#define IMPULSE_OP_GET()		(IMPULSE_OP_PORT->IDR & IMPULSE_OP_PIN)

#define IMPULSE_INTENSITY_MAX	100
#define IMPULSE_VOLT_MAX			BOOSTER_MAX_VOLT

#pragma pack(1)

typedef struct{
	uint16_t 	ms;
	
	uint16_t 	workTime, wCnt;				//工作时间 单位： 秒
	uint8_t		impulseWidth, iCnt;		//脉冲宽度 单位： 10us		5~45 = 50us~450us
	uint16_t	frequency, fCnt;			//频率		 单位： 毫秒		10~500ms = 100~2Hz
	uint16_t	runTime, rCnt;				//运行时间 单位： 毫秒秒	2~99s
	uint16_t	stopTime, sCnt;				//停止时间 单位： 毫秒秒	2~99s
	uint16_t	quickenTime, qCnt;		//加速时间 单位： 毫秒  	300~9900ms
	uint8_t		intensity;						//强度		 单位： 百分比
}impulse_list_t;

#pragma pack()

typedef struct{
	impulse_list_t	list;										//工作方式列表
	u8							nullLoadCnt;						//空负载计数
}impulse_t;


void impulse_init(void);
bool impulse_load_is_null(void);
void impulse_set(u16 iw, u8 freq, u8 rt, u8 st, u16 qt, u8 in, bool restart);
u8 impulse_get_intensity(void);
#endif
