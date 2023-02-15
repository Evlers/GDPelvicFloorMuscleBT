#include "cfSensor.h"
#include "delay.h"
#include "hal_flash.h"
#include "stdio.h"
#include <stdbool.h>

static u32 zeroValue = 0;
static volatile u8 part_id;
static volatile u8 status;

static void dly(u8 n);
static void write(u8 addr, u8 byte);
static u8 read(u8 addr);

void cfsInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	CFS_IO_SDA = 1;
	CFS_IO_SCK = 1;
	
	delay_ms(100);
	write(CFS_REG_SPI_CRTL, 0x20);//复位芯片
	delay_ms(10);
	part_id = read(CFS_REG_PART_ID);//读取芯片ID
	status = read(CFS_REG_STATUS);//读取状态  bit0-Ready 
	
	if(HalFlashDefault() == HalFlashDone)
	{
		HalFlashRead(HAL_FLASH_ZERO_ADDR,(u8 *)&zeroValue,4);
	}
	else 
	{
		cfsZeroCalibration();; 
	}
}

void cfsZeroCalibration (void)
{
	u32 value, count = 0;
	u8 i;
	
	for(i=0;i<5;i++){
		cfsReadData(&value, NULL);
		count += value;
		delay_ms(100);
	}
	
	zeroValue = count / 5;
	HalFlashWrite(HAL_FLASH_ZERO_ADDR,(u8 *)&zeroValue,4);
}

void cfsSleep(void)
{
	write(CFS_REG_CMD, 0xF3);//休眠
}

int cfsReadData(u32 *pass, u16 *temp)
{
	u8 a5,cmd_reg,tmp;
	u32 p;
	static s32 pressure = 0;
	static bool f = true;
	
	if(f){
		a5 = read(0xA5);//读取系统配置
		write(0xA5, a5 & 0xFD);//设置输出为校准的值
		write(CFS_REG_CMD, 0x0A);//开始数据采集  ，组合模式：一次温度采集后立即进行一次传感器压力信号采集
		f = false;
	}
	
//	do{
		cmd_reg = read(CFS_REG_CMD);//读取回命令寄存器
//	}while(cmd_reg & 0x08);//等待采集完成

	if(!(cmd_reg & 0x08)){
	
		//读取数据
		p = 0;
		for(a5=0;a5<3;a5++){
			tmp = read(CFS_REG_DATA_MSB + a5);
			p <<= 8;
			p |= tmp;
		}
		if(p & 0x00800000)p = 0;
		p /= 256;//pa单位
		if((p + 10) >= zeroValue)
			p -= zeroValue - 10;//加入校准值
		else
			p = 0;
		pressure = p;
		
		//读取温度
		if(temp){
			*temp = 0;
			tmp = read(CFS_REG_TEMP_MSB);
			*temp |= tmp;
			*temp <<= 8;
			tmp = read(CFS_REG_TEMP_LSB);
			*temp |= tmp;
			
			*temp /= 287;
		}
		
		f = true;
	}
	
	if(pass)*pass = pressure;
	return 0;
}

static void start(void)
{
	CFS_IO_SDA_OUT;
	CFS_IO_SDA = 1;
	dly(CFS_DLY_TIME);
	CFS_IO_SCK = 1;
	dly(CFS_DLY_TIME);
	CFS_IO_SDA = 0;
	dly(CFS_DLY_TIME);
	CFS_IO_SCK = 0;
	dly(CFS_DLY_TIME);
}

static void iicStop(void)
{
	CFS_IO_SDA_OUT;
	CFS_IO_SDA = 0;
	dly(CFS_DLY_TIME);
	CFS_IO_SCK = 1;
	dly(CFS_DLY_TIME);
	CFS_IO_SDA = 1;
	dly(CFS_DLY_TIME);
	//CFS_IO_SCK = 0;
	dly(CFS_DLY_TIME);
}

static int ack(void)
{
	int ack;
	CFS_IO_SDA_IN;
	dly(CFS_DLY_TIME);
	CFS_IO_SCK = 1;
	dly(CFS_DLY_TIME);
	ack = CFS_IO_SDA_GET ? 1 : 0;
	dly(CFS_DLY_TIME);
	CFS_IO_SCK = 0;
	dly(CFS_DLY_TIME);
	return ack;
}

static void writeByte(u8 byte)
{
	u8 i=0;
	CFS_IO_SDA_OUT;
	for(i=0;i<8;i++)
	{
		if(byte & 0x80)CFS_IO_SDA = 1;
		else CFS_IO_SDA = 0;
		dly(CFS_DLY_TIME);
		CFS_IO_SCK = 1;
		dly(CFS_DLY_TIME);
		CFS_IO_SCK = 0;
		dly(CFS_DLY_TIME);
		byte <<= 1;
	}
}

static u8 readByte(void)
{
	u8 i=0, byte = 0;
	CFS_IO_SDA_IN;
	for(i=0;i<8;i++)
	{
		byte <<= 1;
		CFS_IO_SCK = 1;
		dly(CFS_DLY_TIME);
		if(CFS_IO_SDA_GET)byte |= 0x01;
		CFS_IO_SCK = 0;
		dly(CFS_DLY_TIME);
	}
	return byte;
}

static void write(u8 addr, u8 byte)
{
	//__disable_fiq();
	start();
	writeByte(CFS_ADDR);
	ack();
	writeByte(addr);
	ack();
	writeByte(byte);
	ack();
	iicStop();
	//__enable_fiq();
}

static u8 read(u8 addr)
{
	u8 byte;
	//__disable_fiq();
	start();
	writeByte(CFS_ADDR);
	ack();
	writeByte(addr);
	ack();
	start();
	writeByte(CFS_ADDR|0x01);
	ack();
	byte = readByte();
	ack();
	iicStop();
	//__enable_fiq();
	return byte;
}

static void dly(u8 n)
{
	//delay_us(n);
}


