#include "hal_key.h"

static u32 key = 0;
static u32 keyLongPress;
static u16 longPressTime = 0;

static u32 pollKey = HAL_KEY_SW_ALL;
static u32 waitKey = 0;
static u8 shakeTime;

void HalKeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = HAL_KEY_SW_ALL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void HalKeyTask(void)
{
	if(shakeTime == 0 && pollKey != HAL_KEY_SW_ALL)//如果有按下并且抖动时间过
	{
		if(pollKey == (HAL_KEY_PORT&HAL_KEY_SW_ALL))//按下状态确认
		{
			if(!(pollKey & HAL_KEY_SW_1))
				key |= HAL_KEY_POWER;
			
			waitKey = ~pollKey;
			waitKey &= HAL_KEY_SW_ALL;
			pollKey = HAL_KEY_SW_ALL;
		}
		else
			pollKey = HAL_KEY_SW_ALL;
	}
	
	if((HAL_KEY_PORT & HAL_KEY_SW_ALL) !=  HAL_KEY_SW_ALL && shakeTime == 0 && waitKey == 0)//如果有按钮按下
	{
		pollKey = (HAL_KEY_PORT & HAL_KEY_SW_ALL);//得到按钮状态
		shakeTime = HAL_KEY_SHAKE_TIME;//设置祛抖时间
	}
	
	if(waitKey)//如果有按键按下
	{
		waitKey ^= (HAL_KEY_PORT&waitKey);//清除已经松开的按钮
		if(longPressTime < HAL_KEY_LONG_PRESS_TIME)//如果不到长按时间
			longPressTime++;
		else//长按时间到
		{
			if(waitKey & HAL_KEY_SW_1)
				keyLongPress |= HAL_KEY_POWER;
			else
				keyLongPress &= ~HAL_KEY_POWER;
		}
	}
	else{//所有按钮已经松开
		keyLongPress = 0;
		longPressTime =  0;
	}
	
	if(shakeTime)
		shakeTime--;
}

FlagStatus HalKeyGetShortPress(u8 sw)
{
	FlagStatus s = (sw&key) ? SET:RESET;
	key ^= sw&key;
	return s;
}

FlagStatus HalKeyGetLongPress(u8 sw)
{
	return(sw&keyLongPress) ? SET:RESET;
}

u8 HalKeyGetShortPressNoClear(u8 sw)
{
	return sw & key;
}

u8 HalKeyGetLongPressNoClear(u8 sw)
{
	return sw & keyLongPress;
}




