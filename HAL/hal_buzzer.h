#ifndef _HAL_BUZZER_H_
#define _HAL_BUZZER_H_
#include "stm32f10x.h"
//Active buzzer only 
#define GPIO_PIN									GPIO_Pin_15
#define GPIO_PORT									GPIOA
#define RCC_APB2									RCC_APB2Periph_GPIOA

#define BUZZER_SET								GPIO_PORT->ODR |= GPIO_PIN
#define BUZZER_CLR								GPIO_PORT->ODR &= ~GPIO_PIN

typedef struct{
	u16 Number;//Cycle execution times 
	u16 Period;//Period
	u16 DutyCycle;
	FunctionalState State;//state Enable:Disable
	FunctionalState BuzzerSw;//Buzzer switch 
}Buzzer_t;

/*___________________________ʹ��˵��______________________________
ǰ�ԣ����HALֻ�ʺ���Դ������ 																		 |
ע�⣺��ʼ��֮ǰ��Ҫ�ȳ�ʼ��HalTime Buzzer��Ҫע�ᶨʱ�ص�				 |
-void HalBuzzerInit(void)------------------------------------------|
		��ʼ��������																									 |
																																	 |
-void HalBuzzSetPeriod(u16 Period)---------------------------------|
		���÷������ĵ�ͨ����- һ������Ϊ��ʱ��(HalTime)�Ļص�ʱ��			 |
																																	 |
-void HalBuzzerSet(u8 Number,u16 Period,u16 dutyCucle)-------------|
		����Buzzer��Ӧ�Ĵ����Լ����� �Լ�ռ�ձ� 											 |
		ʾ�� ��																												 |
		Number = 3  Period = 200 dutyCucle = 100 �������� 						 |
		ÿ����100������ͣ100������(����ΪHalTime�ص�ʱ��)							 |
																																	 |
-void HalBuzzerStop(void)------------------------------------------|
										����ֹͣBuzzer																 |
																																	 |
-FunctionalState HalGetBuzzerState(void)---------------------------|
		��ѯBuzzer��ͨ״̬ ENABLE or DISABLE													 |
																																	 |
-void HalBuzzerSwitch(FunctionalState State)-----------------------|
		����Buzzer����-ʹ�ܻ���ʧ��Buzzer ��ʼ����Ĭ����ENABLE				 |
																																	 |
-FunctionalState HalBuzzerSwitchState(void)------------------------|
		��ѯBuzzer����״̬																						 |
___________________________________________________________________|
*/

void HalBuzzerInit(void);
void HalBuzzSetPeriod(u16 Period);
void HalBuzzerSet(u8 Number,u16 Period,u16 dutyCucle);
void HalBuzzerStop(void);
FunctionalState HalGetBuzzerState(void);
void HalBuzzerSwitch(FunctionalState State);
FunctionalState HalBuzzerSwitchState(void);
#endif
