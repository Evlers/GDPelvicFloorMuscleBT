#include "hal_spwm.h"
#include "string.h"

//���PWM
static spwm_t spwm[HAL_SPWM_MAX_CH_NUM];

void HalSPWMInit(void)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++){
		spwm[i].sta = SPWM_SLEEP;
	}
}

//���pwm����
void HalSPWMTask(void)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++)
	{
		if(spwm[i].sta == SPWM_RUN)//�Ƿ�������
		{
			if(spwm[i].sw == SPWM_ON)//���PWM�ǿ�����
			{
				spwm[i].counter++;
				
				if(spwm[i].counter > spwm[i].period)
				{
					if(spwm[i].pulse != 0)
					{
						if(spwm[i].polarity == POLARITY_HIGH)
							*spwm[i].io_port |= spwm[i].io_pin;
						else
							*spwm[i].io_port &= ~spwm[i].io_pin;
					}
					spwm[i].counter = RESET;
				}
				
				if(spwm[i].counter > spwm[i].pulse)
				{
						if(spwm[i].polarity == POLARITY_HIGH)
							*spwm[i].io_port &= ~spwm[i].io_pin;
						else
							*spwm[i].io_port |= spwm[i].io_pin;
				}
			}
			else //���PWM�ǹرյ�
			{
				if(spwm[i].polarity == POLARITY_HIGH)
					*spwm[i].io_port &= ~spwm[i].io_pin;
				else
					*spwm[i].io_port |= spwm[i].io_pin;
			}
		}
	}
}

pwm_id HalSPWMAdd(spwm_t *pwm)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++)
	{
		if(spwm[i].sta == SPWM_SLEEP)//�����PWM����û�б�ʹ��
		{
			memcpy(&spwm[i],pwm,sizeof(spwm_t));//����pwm������Ϣ
			spwm[i].sta = SPWM_RUN;//ǿ������Ϊ����״̬
			pwm->sta = SPWM_RUN;//ǿ������Ϊ����״̬
			return i;//����PWM ID
		}
	}
	return 0xff;
}

u16 HalSPWMPeriodGet(pwm_id id)
{
	return spwm[id].period;
}

void HalSPWMPeriodSet(pwm_id id,u16 period)
{
	spwm[id].period = period;
}

u16 HalSPWMPulseGet(pwm_id id)
{
	return spwm[id].pulse;
}

void HalSPWMPulseSet(pwm_id id,u16 pulse)
{
	spwm[id].pulse = pulse;
}

u16 HalSPWMCountGet(pwm_id id)
{
	return spwm[id].counter;
}

void HalSPWMCountSet(pwm_id id,u16 counter)
{
	spwm[id].counter = counter;
}

spwm_switch HalSPWMSwGet(pwm_id id)
{
	return spwm[id].sw;
}

void HalSPWMCmd(pwm_id id,spwm_switch sw)
{
	spwm[id].sw = sw;
}
