#ifndef _TRAIN_H_
#define _TRAIN_H_
#include "hal_sys.h"

#define TRAIN_PUMP_PORT					GPIOA
#define TRAIN_PUMP_PIN					GPIO_Pin_6
#define TRAIN_PUMP_ON()					TRAIN_PUMP_PORT->BSRR = TRAIN_PUMP_PIN
#define TRAIN_PUMP_OFF()				TRAIN_PUMP_PORT->BRR = TRAIN_PUMP_PIN
#define TRAIN_PUMP_GET()				(TRAIN_PUMP_PORT->ODR & TRAIN_PUMP_PIN)

#define TRAIN_VALVE_PORT				GPIOA
#define TRAIN_VALVE_PIN					GPIO_Pin_5
#define TRAIN_VALVE_ON()				TRAIN_VALVE_PORT->BSRR = TRAIN_VALVE_PIN
#define TRAIN_VALVE_OFF()				TRAIN_VALVE_PORT->BRR = TRAIN_VALVE_PIN

#define TRAIN_STA_INIT					0x01
#define TRAIN_STA_VALVE					0x02

#define TRAIN_INIT_STAB_TIME		100//气压初始化稳定时间
#define TRAIN_INIT_TIMEOUT			15000//气压初始化超时时间
#define TRAIN_VALVE_TIME				2000//泄压时间

#define mmhg_pa(mmhg)						((double)mmhg * 133.3223684)
#define pa_mmhg(pa)							((double)pa / 133.3223684)

typedef struct{
	u8	state;
	u32 setPressure;
	u32 curPressure;
	
	u32 initStabTime;
	u32 initTimeout;
	u32 valveTime;
}train_t;


void train_init(void);
void train_task(void);
void trainDecompression(void);
void trainInitPressure(float mmhg);
float trainPressureGet(void);
void trainDecompression(void);
#endif
