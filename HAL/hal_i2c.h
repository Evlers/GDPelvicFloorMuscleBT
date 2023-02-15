#ifndef _HAL_I2C_H_
#define _HAL_I2C_H
#include "stm32f10x.h"
#define HAL_IIC_PORT_CLOCK								RCC_APB2Periph_GPIOB
#define HAL_IIC_PORT											GPIOB
#define HAL_IIC_SCL												GPIO_Pin_10
#define HAL_IIC_SDA												GPIO_Pin_11
#define HAL_IIC_SCL_BIT										2//设置引脚位置
#define HAL_IIC_SDA_BIT										3

#define HAL_IIC_SCL_SET										HAL_IIC_PORT->ODR |= HAL_IIC_SCL
#define HAL_IIC_SCL_CLR										HAL_IIC_PORT->ODR &= ~HAL_IIC_SCL
#define HAL_IIC_SDA_SET										HAL_IIC_PORT->ODR |= HAL_IIC_SDA
#define HAL_IIC_SDA_CLR										HAL_IIC_PORT->ODR &= ~HAL_IIC_SDA
#define HAL_IIC_SDA_DAT										HAL_IIC_PORT->IDR & HAL_IIC_SDA
#define HAL_IIC_SDA_IN										HAL_IIC_PORT->CRH &= ~((u32)0x0f << (HAL_IIC_SDA_BIT *4));HAL_IIC_PORT->CRH |= ((u32)8 << (HAL_IIC_SDA_BIT *4))//上拉/下拉输入
#define HAL_IIC_SDA_OUT										HAL_IIC_PORT->CRH &= ~((u32)0x0f << (HAL_IIC_SDA_BIT *4));HAL_IIC_PORT->CRH |= ((u32)3 << (HAL_IIC_SDA_BIT *4))//推挽输出模式

void HalI2cInit(void);
void HalI2cStart(void);
void HalI2cStop(void);
u8 HalI2cWaitAck(void);
void HalI2cWriteByte(u8 txd);
u8 HalI2cReadByte(unsigned char ack);
#endif
