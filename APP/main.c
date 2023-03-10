#include "hal_sys.h"
#include "hal_key.h"
#include "hal_usart.h"
#include "hal_flash.h"
#include "hal_time.h"
#include "hal_buzzer.h"

#include "train.h"
#include "impulse.h"
#include "fun.h"
#include "ble.h"
#include "ble_commun.h"
#include "log.h"
#include "stdio.h"
#include "delay.h"
#include "hal.h"
#include "booster.h"
#include "cfsensor.h"

#include <string.h>

int main (void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	
	// SystemInit();
	HalSysClockInit();
	HalTimeInit(72,1000);
	ble_com_init();
	ble_init();
	delay_ms(500);
	// HalSleep();

	// ble_wake();
	HalKeyInit();
	HalTimeRegisteringCallbacks(HalKeyTask, 1);
	
	hal_initialzation();
	booster_config();
	cfsInit();
	
	HalFlashFlag();

	// delay_ms(500);
	
	fun_init();
	
	train_init();
	impulse_init();
	
	LOG_D("run...");
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	
	// IWDG_Init(IWDG_Prescaler_4, 20000);
	while(1)
	{
		fun_task();
		train_task();
		booster_task();
		ble_task();
		IWDG_Feed();
	}
}
