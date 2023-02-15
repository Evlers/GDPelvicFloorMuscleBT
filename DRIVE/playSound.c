#include "playSound.h"
#include "string.h"
#include "math.h"

#define YX3P_SCK								PBout(11)
#define YX3P_SDA								PBout(10)
#define YX3P_VPP								PBin(2)

static PlaySoundTypeDef *SoundCache[PLAY_SOUND_CACHE_MAX];
#if(CREATE_CACHE_MEMORY)
static PlaySoundTypeDef Cache[PLAY_SOUND_CACHE_MAX];
#endif

void PlaySoundConfig(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOA时钟
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_Init(GPIOB, &GPIOInitStruct);
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_Init(GPIOB, &GPIOInitStruct);
	YX3P_SCK = 1;
	YX3P_SCK = 0;
	YX3P_SDA = 1;
	YX3P_SDA = 0;
	memset(SoundCache, 0, sizeof(SoundCache));
	#if(CREATE_CACHE_MEMORY)
	memset(Cache, 0, sizeof(Cache));
	#endif
}

void PlaySoundTask(void)
{
	static u32 pulseCount = 0;			//脉冲计数
	static u32 SpaceTimeCount = 0;	//间隔时间计数
	static int i = 0;
	static u8 flag = 0;
	/*for(i = 0; i < PLAY_SOUND_CACHE_MAX; i++)//判断有效的音频
	{
		if(SoundCache[i] != 0)			
		{
			if(SoundCache[i]->playStart)			//判断音频是否正在播放
				break;
		}
	}*/
	if((SoundCache[i] != 0) && (SoundCache[i]->playStart != 0))		//播放音频
	{
		if(flag)			//复位脉冲结束
		{
			flag = 0;
			YX3P_SCK=0;
			return;
		}
		if(pulseCount < (SoundCache[i]->SoundNumber * 2))						//判断播放音频号
		{
			if(pulseCount % 2)
				YX3P_SDA=0;
			else
				YX3P_SDA=1;
			pulseCount++;
		}
		else																												//音频脉冲发送完成
		{
			#if(PLAY_SOUND_USR_BUS_BUSY)
			if(!YX3P_VPP)
			{
				return;
			}
			#endif
			if(SpaceTimeCount < SoundCache[i]->SpaceTime)			//判断间隔等待
			{
				SpaceTimeCount++;
			}
			else						//延时完成
			{
				SpaceTimeCount = 0;
				pulseCount = 0;
				if(SoundCache[i]->PlayNum - 1)			//未播放完成
				{
					SoundCache[i]->PlayNum--;
					YX3P_SCK=1;										//复位脉冲
					flag = 1;
				}
				else
				{
					SoundCache[i] = 0;
				}
			}
		}
	}
	else														//没有音频正在播放
	{
		for(i = 0; i < PLAY_SOUND_CACHE_MAX; i++)				//从缓冲选择需要播放的音频
		{
			if(SoundCache[i] != 0)			//判断有效的音频
			{
				break;
			}
		}
		if(i != PLAY_SOUND_CACHE_MAX)
		{
			SoundCache[i]->playStart = 1;	//开始播放音频
			YX3P_SCK=1;										//复位脉冲
			YX3P_SCK=0;
			YX3P_SCK=1;										//复位脉冲
			flag = 1;
		}
		else
			i = 0;
	}
}

void PlaySoundClose(u8 SoundNumber)
{
	for(int i = 0; i < PLAY_SOUND_CACHE_MAX; i ++)
	{
		if(SoundCache[i] != 0)
		{
			if(SoundCache[i]->SoundNumber == SoundNumber)
			{
				SoundCache[i]->PlayNum = 1;
				break;
			}
		}
	}
}

u8 PlaySound(PlaySoundTypeDef *sound)
{
	for(int i = 0; i < PLAY_SOUND_CACHE_MAX; i ++)
	{
		if(SoundCache[i] == 0)
		{
			#if(CREATE_CACHE_MEMORY)
			memcpy(&Cache[i], sound, sizeof(PlaySoundTypeDef));
			SoundCache[i] = &Cache[i];
			SoundCache[i]->playStart = 0;		//等待播放
			#else
			SoundCache[i] = sound;
			SoundCache[i]->playStart = 0;		//等待播放
			#endif
			return 0;
		}
	}
	return 1;
}

//Number:数字(带一位小数，num(Max=999.9) * 10)  unit:单位
void PlaySoundNumber(u32 number,u8 unit)			
{
	PlaySoundTypeDef sound[10];
	PlaySoundTypeDef *p = sound;
	if(number > 9999.9) return;
	memset(sound, 0, sizeof(sound));
	for(int i = 0; i < 10; i++)
	{
		sound[i].PlayNum = 1;
		sound[i].SpaceTime = 2;
	}
	for(int i = 0; i < 5; i++)
	{
		int tmp = pow(10, 5 - i - 1);
		if(number / tmp)
		{
			for(int j = 5 - i; j > 0; j--)
			{
				int tmp = number / pow(10, j - 1);
				p->SoundNumber = (tmp % 10) + 2;		//数字
				if(p->SoundNumber != 0 && j >= 3)		//个位零不念单位
				{
					p++;
					p->SoundNumber = 12 + j - 3;
				}
				else if(j == 2)				//个位后面加 点
				{
					p++;
					p->SoundNumber = 15;			//点
				}
				p++;
			}
			break;
		}
	}
	if(unit)
	{
		p->SoundNumber = unit;		//单位
		p++;
	}
	PlaySeriesSounds(sound,  p - sound);
}

bool PlaySeriesSounds(PlaySoundTypeDef *sound, int len)
{
	int i,j;
	for(j = 0; j < PLAY_SOUND_CACHE_MAX; j++)
	{
		if((SoundCache[j] == 0) && ((j + len) <= PLAY_SOUND_CACHE_MAX))
		{
			for(i = 0; i < len; i++)
			{
				if(SoundCache[j + i] != 0)
				{
					j += i + 1;
					break;
				}
			}
			if(i == len) 										//成功找到连续内存
			{
				for(int x = 0; x < len; x++)
				{
					#if(CREATE_CACHE_MEMORY)
					memcpy(&Cache[x + j], &sound[x], sizeof(PlaySoundTypeDef));
					SoundCache[x + j] = &Cache[x + j];
					SoundCache[x + j]->playStart = 0;		//等待播放
					#else
					SoundCache[x + j] = &sound[x];
					SoundCache[x + j]->playStart = 0;		//等待播放
					#endif
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

