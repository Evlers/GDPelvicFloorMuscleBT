#include "train.h"
#include "cfsensor.h"
#include "stdio.h"
#include "hal.h"

//盆底肌训练(气压监测)

train_t train;

static void pressureRead(void);
static void trainTimeCallback(void);
static void pumpAndValveManage(void);

void train_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = TRAIN_PUMP_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRAIN_PUMP_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = TRAIN_VALVE_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRAIN_VALVE_PORT, &GPIO_InitStruct);
	
	train.state = 0;
	train.setPressure = 0;
	train.curPressure = 0;
	train.initStabTime = 0;
	train.initTimeout = 0;
	train.valveTime = 0;
	HalTimeRegisteringCallbacks(trainTimeCallback, 1);
	
	// trainDecompression();
}

void train_task(void)
{
	pumpAndValveManage();
	pressureRead();
}

//获取当前气压  单位：mmHg
float trainPressureGet(void)
{
	return pa_mmhg(train.curPressure);
}

//泄压
void trainDecompression(void)
{
	train.valveTime = TRAIN_VALVE_TIME;
	train.state |= TRAIN_STA_VALVE;
}

//初始化气压 单位：mmHg
void trainInitPressure(float mmhg)
{
	train.valveTime = TRAIN_VALVE_TIME;
	train.initStabTime = TRAIN_INIT_STAB_TIME;
	train.initTimeout = TRAIN_INIT_TIMEOUT;
	train.setPressure = mmhg_pa(mmhg);
	train.state |= TRAIN_STA_INIT | TRAIN_STA_VALVE;
}

static void pumpAndValveManage(void)
{
	if(train.state & TRAIN_STA_VALVE){//开启泄压阀门
		TRAIN_VALVE_ON();
		if(train.valveTime == 0){
			TRAIN_VALVE_OFF();
			train.state &= ~TRAIN_STA_VALVE;
		}
		return ;
	}
	
	if(train.state & TRAIN_STA_INIT){//初始化气压
		if(train.initStabTime == 0){
			if(TRAIN_PUMP_GET()){
				TRAIN_PUMP_OFF();
				train.initStabTime = TRAIN_INIT_STAB_TIME;
				
			} else {
				if(train.curPressure < train.setPressure && train.initTimeout){
					TRAIN_PUMP_ON();
					train.initStabTime = TRAIN_INIT_STAB_TIME;
				} else {
					train.state &= ~TRAIN_STA_INIT;
				}
			}
			
		}
	}
}

static void pressureRead(void)
{
	u32 press;
	cfsReadData(&press, NULL);
	train.curPressure = press;//(press + train.curPressure * 15) / 16;
}

static void trainTimeCallback(void)
{
	if(train.valveTime)train.valveTime--;
	if(train.initTimeout)train.initTimeout--;
	if(train.initStabTime)train.initStabTime--;
}
