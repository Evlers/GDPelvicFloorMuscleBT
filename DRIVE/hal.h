#ifndef _HAL_H_
#define _HAL_H_
#include "hal_sys.h"
#include "hal_key.h"
#include "hal_adc.h"
#include "hal_time.h"
#include "hal_buzzer.h"

#define HAL_ADC_REF                                                             3.3                                     //ADC参考电压
#define HAL_ADC_CH_NUM                                                          2                                       //ADC通道数

#define BAT_VOL_BUS_R1                                                          100                                     //电池电压测量分压电阻
#define BAT_VOL_BUS_R2                                                          68                                     
#define BAT_VOL_VALUE(vol)                                                      ((double)vol * ((double)BAT_VOL_BUS_R1 / ((double)BAT_VOL_BUS_R1 + (double)BAT_VOL_BUS_R2)) * 4095.0 / (double)HAL_ADC_REF)
#define BAT_VALUE_VOL(value)																										((double)HAL_ADC_REF / 4095.0 * (double)value / (double)BAT_VOL_BUS_R2 * ((double)BAT_VOL_BUS_R1 + (double)BAT_VOL_BUS_R2))

#pragma pack(push, 1)                               //1字节对齐
typedef struct 
{
	u16 out_vol;                                        //输出电压
	u16 bat_vol;                                        //电池电压
}hal_adc_meas_typedef;
#pragma pack(pop)                                   //结束

typedef struct
{
	hal_adc_meas_typedef analong;
	

}hal_typedef;

extern hal_typedef hal;

void hal_initialzation(void);
void hal_TIM1_OC_config(u16 period);
void hal_TIM1_OC1_duty_set(u16 duty);
u16 hal_TIM1_OC1_duty_get(void);
void hal_TIM3_OC_config(u16 period);
void hal_TIM3_OC1_duty_set(u16 duty);
u16 hal_TIM3_OC1_duty_get(void);

void IWDG_Init(u8 prer,u16 rlr) ;
void IWDG_Feed(void);
void HalSleep(void);
#endif
