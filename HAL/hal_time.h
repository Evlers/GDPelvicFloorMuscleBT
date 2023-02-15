#ifndef _HAL_TIME_H_
#define _HAL_TIME_H_
#include "stm32f10x.h"

#define HAL_TIME_MAX_CB_NUM										20//�������ص�����

typedef void (*HalTimeCallBack_t) ( void );

/*______________________ʹ��˵��______________________________
-void HalTimeInit(u16 Prescaler, u16 Period)------------------|
		��ʼ����ʱ��	Prescaler:Ԥ��Ƶ  	Period:��������												|
																															|
-HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack)-----|
		ע�ᶨʱ�ص�																										|
																															|
-u8 HalTimeRemoveCallback (HalTimeCallBack_t CallBack)--------|
		ɾ����ʱ�ص� ����ֵ- 0:OK 1:Error(δ�ҵ��Ѿ�ע��ĺ���)	        		|
______________________________________________________________|
*/

typedef enum{
HAL_TIME_MODE_RELOAD,//��װ
HAL_TIME_MODE_ONCE,//һ��
}HalTimeCbMode;//�ص�ģʽ

typedef struct{
u16 count;//������
u16 prescaler;//Ԥ��Ƶ
HalTimeCallBack_t cb;//�ص�����
HalTimeCbMode mode;//�ص�ģʽ
}HalTime_t;

void HalTimeInit(u16 Prescaler, u16 Period);
u8 HalTimeExStart(HalTimeCallBack_t CallBack,u16 prescaler);
u8 HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack,u16 prescaler);
u8 HalTimeCancelCallback (HalTimeCallBack_t CallBack);
#endif

