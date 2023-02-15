#ifndef _HAL_SPWM_H_
#define _HAL_SPWM_H_
#include "stm32f10x.h"

#define HAL_SPWM_MAX_CH_NUM									20//����������PWMͨ��������

typedef u8 pwm_id;

typedef enum{
SPWM_OFF = 0,
SPWM_ON = 1,
}spwm_switch;//����PWM

typedef enum{
POLARITY_HIGH = 0,
POLARITY_LOW,
}polarity_sta;//�������

typedef enum{
SPWM_SLEEP = 0,//�����PWM�����ߵ�״̬��Ŀǰû��ʹ��,�ɱ�ʹ�ã�
SPWM_RUN,//�����PWM���ڹ��� ��Ŀǰ�������PWM���ɱ�ʹ�ã�
}spwm_reg_sta;

typedef struct{
u16 counter;//������
u16 period;//����
u16 pulse;//ռ�ձ�
volatile u32 *io_port;//���Ŷ˿�
u32 io_pin;//����λ
polarity_sta polarity;//�������
spwm_switch sw;//���pwn����
spwm_reg_sta sta;//״̬�Ĵ���
}spwm_t;//���PWM�Ĵ���

/*__________________________________________________________________________
-void HalSPWMInit(void)-----------------------------------------------------|
		��ʼ�����PWM���� ȷ������PWMͨ���ر��������״̬																						|
																																						|
-void HalSPWMTask(void)-----------------------------------------------------|
		�����PWMʱ������  ����ʹ��HalTimeע��һ���ص� ��������Spwm��ʱ��							  			    			|
																																						|
-pwm_id HalSPWMAdd(spwm_t *pwm)---------------------------------------------|
		���һ��PWMͨ��  ����pwm=spwm_t(��ʼ������)																					|
		����ֵpwm_id��ÿ��spwm����ݱ�ʶ(���)																								|
																																						|
-void HalSPWMCmd(pwm_id id,spwm_switch sw)----------------------------------|
		ʹ��orʧ�ܴ�ID��pwm�������																												|
		����id = ��Ӻ󷵻ص�ID																													|
		����sw = �������߹ر�																														|
																																						|
-void HalSPWMPulseSet(pwm_id id,u16 pulse)----------------------------------|
		ռ�ձ�����  																																	|
		����id = ��ӵ�pwm����ֵ																												|
		����pulse = ����ռ�ձ�																													|
																																						|
-void HalSPWMPeriodSet(pwm_id id,u16 period)--------------------------------|
		��������																																		|
		���� id = ���pwm�ķ���ֵ																												|
		����period = PWM����																												|
																																						|
-void HalSPWMCountSet(pwm_id id,u16 counter)--------------------------------|
		����������																																		|
		���� id = ���PWM�ķ���ֵ																												|
		����counter = ���ü�������ֵ																											|
																																						|
-u16 HalSPWMPeriodGet(pwm_id id)--------------------------------------------|
		��ѯpwm����																																	|
		����id = addr spwm return value																						|
		����ֵ������																																	|
																																						|
-u16 HalSPWMPulseGet(pwm_id id)---------------------------------------------|
		��ѯռ�ձ�																																		|
																																						|
-u16 HalSPWMCountGet(pwm_id id)---------------------------------------------|
		��ѯ����ֵ																																		|
																																						|
-spwm_switch HalSPWMSwGet(pwm_id id)----------------------------------------|
		��ѯspwm�Ŀ���״̬																															|
____________________________________________________________________________|
*/

void HalSPWMInit(void);
void HalSPWMTask(void);
pwm_id HalSPWMAdd(spwm_t *pwm);
void HalSPWMCmd(pwm_id id,spwm_switch sw);
void HalSPWMPulseSet(pwm_id id,u16 pulse);
void HalSPWMPeriodSet(pwm_id id,u16 period);
void HalSPWMCountSet(pwm_id id,u16 counter);

u16 HalSPWMPeriodGet(pwm_id id);
u16 HalSPWMPulseGet(pwm_id id);
u16 HalSPWMCountGet(pwm_id id);
spwm_switch HalSPWMSwGet(pwm_id id);
#endif
