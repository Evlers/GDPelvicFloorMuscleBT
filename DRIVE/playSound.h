#ifndef _PLAYSOUND_H_
#define _PLAYSOUND_H_

#include "stm32f10x.h"
#include "hal_sys.h"

#define PLAY_SOUND_CACHE_MAX																	32				//音频缓冲数
#define CREATE_CACHE_MEMORY																		1				//创建缓冲内存
#define PLAY_SOUND_USR_BUS_BUSY																1				//使用总线忙检测
#define PLAY_SOUND_UNIT_YUAN																	16			//元
#define PLAY_SOUND_UNIT_DU																		30			//度
#define PLAY_SOUND_UNIT_W																			35			//瓦

typedef struct
{
	u8 playStart;				//开始播放标志
	u8 SoundNumber;			//播放的音频号
	u8 PlayNum;					//播放次数
	u16 SpaceTime;				//播放间隔时间
}PlaySoundTypeDef;

void PlaySoundConfig(void);
void PlaySoundTask(void);
u8 PlaySound(PlaySoundTypeDef *sound);
bool PlaySeriesSounds(PlaySoundTypeDef *sound, int len);
void PlaySoundNumber(u32 number,u8 unit);
void PlaySoundClose(u8 SoundNumber);

#endif
