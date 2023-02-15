#ifndef _HAL_EXTI_H_
#define _HAL_EXTI_H_
#include "hal_sys.h"

#define HAL_EXTI_MAX_CB_NUM				20//���ص����� (һ��IO�ɶ���ص�)

typedef enum{
HAL_EXTI_GPIOA = 0,
HAL_EXTI_GPIOB = 1,
HAL_EXTI_GPIOC = 2,
HAL_EXTI_GPIOD = 3,
HAL_EXTI_GPIOE = 4,
HAL_EXTI_GPIOF = 5,
HAL_EXTI_GPIOG = 6,
}HalGpioTypeDef;

typedef void (*HalExtiCallBack_t)(void);//�ⲿ�жϻص�

typedef struct{
HalExtiCallBack_t cb;//�ص�����
u8 line;//��Ҫ�ص���Pin
}HalExti_t;

//�ⲿ�жϳ�ʼ��ע��
//����GPIOx:����HalGpioTypeDefö�ٶ��� 
//����PINx:0~15
//����IOMode:�ο�EXTITrigger_TypeDef�ඨ�� 
//����Trigger:�ο�EXTITrigger_TypeDef (ѡ��������)
//����CB:�жϻص�����
void HalExtiInit(HalGpioTypeDef GPIOx,
									 	u8 PINx,
											GPIOMode_TypeDef IOMode,
												EXTITrigger_TypeDef Trigger,
													HalExtiCallBack_t CB);

#endif

