#include "hal_time.h"
#include "string.h"
//ͨ�ö�ʱ���ж�

HalTime_t HalTime[HAL_TIME_MAX_CB_NUM];//��ʱ���ص�

//ͨ�ö�ʱ��4��ʼ��
//Prescaler:ʱ�ӷ�Ƶ
//Period:�Զ���װֵ
void HalTimeInit(u16 Prescaler, u16 Period)
{
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	NVIC_InitTypeDef NVICInitStruct;
	
	memset(HalTime,0,HAL_TIME_MAX_CB_NUM);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIMInitstruct.TIM_Prescaler = Prescaler;//��Ƶ
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIMInitstruct.TIM_Period = Period;//����ֵ
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM4,&TIMInitstruct);//��ʼ����ʱ��
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//ʹ�ܶ�ʱ�������ж�
	TIM_Cmd(TIM4,ENABLE);//ʹ�ܶ�ʱ��
	
	NVICInitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0x02;//�ж����ȼ�
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ���ж�
	NVIC_Init(&NVICInitStruct);//��ʼ���ж�������
}

//��ʱ����
//����CallBack:�ص�����
//����prescaler:Ԥ��Ƶ
//����ֵ: 1:û��ʣ�µĻ��� 0:ע��ɹ�
u8 HalTimeExStart(HalTimeCallBack_t CallBack,u16 prescaler)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//���յ�λ��
	{
		if(HalTime[i].cb == RESET){//�����λ
			HalTime[i].mode = HAL_TIME_MODE_ONCE;//���õ���һ��
			HalTime[i].prescaler = prescaler;//���÷�Ƶ
			HalTime[i].count = RESET;//�������
			HalTime[i].cb = CallBack;//д��ص�
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}

//ע��ص�
//����CallBack:�ص�����
//����prescaler:Ԥ��Ƶ
//����ֵ: 1:û�п��ûص��ռ� 2:�Ѿ�ע���  0:���óɹ�
u8 HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack,u16 prescaler)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//�����ûע���
		if(HalTime[i].cb == CallBack)
			break;
	if(i != HAL_TIME_MAX_CB_NUM)return 2;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//���յ�λ��
	{
		if(HalTime[i].cb == RESET){//�����λ
			HalTime[i].mode = HAL_TIME_MODE_RELOAD;//������װģʽ
			HalTime[i].prescaler = prescaler;//���÷�Ƶ
			HalTime[i].count = RESET;//�������
			HalTime[i].cb = CallBack;//д��ص�
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}
//ȡ���ص�
//����ֵ�� 1:����,û���ҵ��ûص� 0:�Ƴ����
u8 HalTimeCancelCallback (HalTimeCallBack_t CallBack)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)
	{
		if(HalTime[i].cb == CallBack){
			HalTime[i].cb = NULL;
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}
//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		for(u8 i=0;i<HAL_TIME_MAX_CB_NUM;i++)
		{
			if(HalTime[i].cb){
				HalTime[i].count++;//�ݼӼ�����
				if(HalTime[i].count >= HalTime[i].prescaler){
					HalTime[i].cb();//�ص�
					if(HalTime[i].mode == HAL_TIME_MODE_ONCE)//���ֻ��Ҫ����һ��
						HalTime[i].cb = NULL;//������ص�
					else
						HalTime[i].count = RESET;//���������
				}
			}
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ
}


