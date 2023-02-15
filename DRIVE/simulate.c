#include "simulate.h"
#include "hal_adc.h"

u8 adcChannel[]={4,5};
u16 adcValue[sizeof(adcChannel)];

void simulateInit(void)
{
	HalAdcInit(adcChannel,adcValue,sizeof(adcChannel));
}

u16 simulateRead(simulate_t site)
{
	return adcValue[site];
}

u16 simulateReadAverage(simulate_t site)
{
	u32 value = 0;
	for(u8 i=0;i<32;i++)
	{
		adcValue[site] = 0x8000;
		while(adcValue[site] & 0x8000);
		value += adcValue[site];
	}
	return value / 32;
}
