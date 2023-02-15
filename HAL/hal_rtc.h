#ifndef __RTC_H
#define __RTC_H	    
#include "hal_sys.h"
#include "delay.h"

typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}HalRTCTime_t;	

typedef void (*HalRtcCallbacks_t ) (HalRTCTime_t *Calendar);

u8 HalRTCInit(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
HalRTCTime_t *HalRTCGetTimer(void); //��ѯRTCʱ��
u8 HalRTCIsLeapYear(u16 year);//ƽ��,�����ж�
u8 HalRTCAlarmSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//��������
u8 HalRTCGet(void);         //����ʱ��   
u8 HalRTCGetWeek(u16 year,u8 month,u8 day);
u8 HalRTCSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��
void HalRtcRegisteringCallbacks(HalRtcCallbacks_t Callbacks);//ע��RTC��Ļص�
#endif


