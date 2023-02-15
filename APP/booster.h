#ifndef _BOOSTER_H_
#define _BOOSTER_H_
#include "hal_sys.h"

#define BOOSTER_AD_EN_PORT				GPIOA
#define BOOSTER_AD_EN_PIN					GPIO_Pin_1
#define BOOSTER_AD_EN()						BOOSTER_AD_EN_PORT->BSRR = BOOSTER_AD_EN_PIN
#define BOOSTER_AD_DS()						BOOSTER_AD_EN_PORT->BRR = BOOSTER_AD_EN_PIN

#define BOOSTER_DSIC_PORT					GPIOA
#define BOOSTER_DISC_PIN					GPIO_Pin_7
#define BOOSTER_DISC_EN()					BOOSTER_DSIC_PORT->BSRR = BOOSTER_DISC_PIN
#define BOOSTER_DISC_DS()					BOOSTER_DSIC_PORT->BRR = BOOSTER_DISC_PIN

#define BOOSTER_VOLT_AD_CH				4

#define BOOSTER_FREQ                                                (20000)                                                //升压频率
#define BOOSTER_PREIOD                                              (HAL_SYSTEM_CLOCK / BOOSTER_FREQ)                       //周期

#define BOOSTER_V_BUS_R1                                            150000
#define BOOSTER_V_BUS_R2                                            4700
#define BOOSTER_VALUE_VOLT(ad)																			((double)HAL_ADC_REF / 4096.0 * (double)ad / (double)BOOSTER_V_BUS_R2 * ((double)BOOSTER_V_BUS_R1 + (double)BOOSTER_V_BUS_R2))
#define BOOSTER_VOLT_VALUE(vol)                                     ((double)vol * ((double)BOOSTER_V_BUS_R1/((double)BOOSTER_V_BUS_R1 + (double)BOOSTER_V_BUS_R2)) * 4095 / (double)HAL_ADC_REF)

#define BOOSTER_MAX_VOLT					50
#define BOOSTER_ALLOW_OFFSET			4

typedef struct
{
 q15 kp;    //比例系数
 q15 ki;    //积分系数
 q15 kd;    //微分系数
 s32 f;    //积分
 q15 errl;   //上一时刻微分
}pid_q15_type;

typedef struct{
	u32 volt;
	pid_q15_type pid;
}booster_t;

void booster_config(void);
void booster_task(void);
void booster_volt_set(u8 valot);
#endif
