#include "hal_i2c.h"
#include "delay.h"

void HalI2cInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(HAL_IIC_PORT_CLOCK, ENABLE);//使能GPIOB时钟
	
  GPIO_InitStructure.GPIO_Pin = HAL_IIC_SCL | HAL_IIC_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HAL_IIC_PORT, &GPIO_InitStructure);//初始化
	HAL_IIC_SCL_SET;
	HAL_IIC_SDA_SET;
}
void HalI2cStart(void)
{
	HAL_IIC_SDA_OUT;
	HAL_IIC_SDA_SET;	  	  
	HAL_IIC_SCL_SET;
	delay_us(4);
 	HAL_IIC_SDA_CLR;
	delay_us(4);
	HAL_IIC_SCL_CLR;
}
void HalI2cStop(void)
{
	HAL_IIC_SDA_OUT;
	HAL_IIC_SDA_CLR;
	HAL_IIC_SCL_SET;
 	delay_us(4); 
	HAL_IIC_SDA_SET;
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 HalI2cWaitAck(void)
{
	u8 ucErrTime=0;
	HAL_IIC_SDA_IN;      //SDA设置为输入  
	HAL_IIC_SDA_SET;delay_us(1);	   
	HAL_IIC_SCL_SET;delay_us(1);	 
	while(HAL_IIC_SDA_DAT)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			HalI2cStop();
			return 1;
		}
	}
	HAL_IIC_SCL_CLR;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void HalI2cAck(void)
{
	HAL_IIC_SCL_CLR;
	HAL_IIC_SDA_OUT;
	HAL_IIC_SDA_CLR;
	delay_us(2);
	HAL_IIC_SCL_SET;
	delay_us(2);
	HAL_IIC_SCL_CLR;
	delay_us(2);
}
//不产生ACK应答		    
void HalI2cNAck(void)
{
	HAL_IIC_SCL_CLR;
	HAL_IIC_SDA_OUT;
	HAL_IIC_SDA_SET;
	delay_us(2);
	HAL_IIC_SCL_SET;
	delay_us(2);
	HAL_IIC_SCL_CLR;
}
void HalI2cWriteByte(u8 txd)
{
	u8 t;   
	HAL_IIC_SDA_OUT; 	    
	HAL_IIC_SCL_CLR;
	for(t=0;t<8;t++)
	{        
		if(txd & 0x80)
			HAL_IIC_SDA_SET;
		else
			HAL_IIC_SDA_CLR;
		txd<<=1; 	  
		delay_us(2);
		HAL_IIC_SCL_SET;
		delay_us(2); 
		HAL_IIC_SCL_CLR;	
		delay_us(2);
	}
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 HalI2cReadByte(unsigned char ack)
{
	unsigned char i,receive=0;
	HAL_IIC_SDA_IN;
  for(i=0;i<8;i++ )
	{
		HAL_IIC_SCL_CLR; 
		delay_us(2);
		HAL_IIC_SCL_SET;
		receive<<=1;
		if(HAL_IIC_SDA_DAT)receive++;   
		delay_us(1); 
	}					 
	if (!ack)
			HalI2cNAck();
	else
			HalI2cAck();  
	return receive;
}
