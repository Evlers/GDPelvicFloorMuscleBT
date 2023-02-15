#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_
#include "hal_sys.h"

/*___________________________________________ʹ��˵��_______________________________________________
-void HaladcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum)--------------------------------|
			���������ڳ�ʼ��ADͨ��-��ʼ�����֮��ChannelBuff����᲻�ϸ��¶�ӦChannelTable�����б������ 	|
			ChannelTable :��Ҫ������ADͨ�� �и�������ʽд�� 																						 	|
			ChannelBuff  :��ӦChannelTable����˳���AD�ɼ����� 																					 	|
			ChannelNum	 :ʹ�õ�ͨ������																																 	|
			|ʾ��:____________________________________________________________________										|
			|		 ChannelTable = {0,3,2,1}																							|										|
			|		 ChannelBuff = AdData[sizeof(ChannelTable)]														|									 	|
			|		 ChannelNum = sizeof(ChannelTable)																		|									 	|
			|																																					|										|
			|���ǵ�˳���� :																														|										|
			|							ChannelBuff[0] = ADC1ͨ��0 AIN0������(ChannelTable[0] = 0)	|										|
			|							ChannelBuff[1] = ADC1ͨ��3 AIN3������(ChannelTable[1] = 3)	|										|
			|							ChannelBuff[2] = ADC1ͨ��2 AIN2������(ChannelTable[1] = 2)	|										|
			|							ChannelBuff[3] = ADC1ͨ��1 AIN1������(ChannelTable[1] = 1)	|										|
			|_________________________________________________________________________|										|
																																																		|
-u16 HalAdcRead(u8 ADC_Channel)---------------------------------------------------------------------|
			��ȡ��Ӧͨ������																																							|
			ADC_Channel��Ӧ��ʼ��ChannelTable������б�˳��																								|
																																																		|
-void HalAdcSetConvertCallback(void (*func)(void))--------------------------------------------------|
			����ת����ɺ�Ļص�																																					|
																																																		|
-void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len)----------------------------------------------|
			��ȡ��Ӧͨ����Len������																																				|
____________________________________________________________________________________________________|
*/

void HalAdc2Init(u8 *ChannelTable, int ChannelNum);
void HalAdcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum);
u16 HalAdcRead(u8 ADC_Channel);
u16 HalAdc2Read(u8 channel);
void HalAdcSetConvertCallback(void (*func)(void));
void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len);

#endif
