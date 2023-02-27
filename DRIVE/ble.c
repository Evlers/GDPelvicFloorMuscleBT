#include "ble.h"
#include "ble_commun.h"
#include "hal_usart.h"
#include "delay.h"
#include "log.h"
#include <stdio.h>

static void usartIdleCallback(u8 *Buf, u16 Len);
	
void ble_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = BLE_WAKE_PIN | BLE_RESET_PIN | BLE_RELOAD_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLE_GPIO_PORT, &GPIO_InitStruct);
	
	delay_ms(100);
	
	BLE_RELOAD_SET;
	BLE_WAKE_CLR;
	
	BLE_RESET_CLR;
	delay_ms(100);
	BLE_RESET_SET;
	
	HalUSARTInit(BLE_USART_PORT, 57600, usartIdleCallback);
	delay_ms(200);
	
	HalUSARTString(BLE_USART_PORT, "+++a\r\n");
	delay_ms(200);
	char at_name[32];
	snprintf(at_name, sizeof(at_name), "AT+NAME=%s_%s\r\n", BLE_NAME, VERSION);
	HalUSARTString(BLE_USART_PORT, at_name);
	delay_ms(200);
	LOG_D("%s", at_name);
	HalUSARTString(BLE_USART_PORT, "AT+Z\r\n");
	delay_ms(200);
}

void ble_task(void)
{
	
}

void ble_sleep(void)
{
	BLE_RESET_CLR;
	delay_ms(100);
	BLE_RESET_SET;
	delay_ms(200);
	HalUSARTString(BLE_USART_PORT, "+++a\r\n");
	delay_ms(200);
	BLE_WAKE_SET;
	HalUSARTString(BLE_USART_PORT, "AT+HIBERNATE\r\n");
	delay_ms(1000); // 等待DMA发送完毕
}

void ble_wake(void)
{
	BLE_WAKE_CLR;
	delay_ms(200);
	HalUSARTString(BLE_USART_PORT, "AT+Z\r\n");
	delay_ms(200);
}

void ble_send(u8 *data, u16 len)
{
	HalUSARTWriteData(BLE_USART_PORT, data, len);
}

static void usartIdleCallback(u8 *Buf, u16 Len)
{
	ble_com_pack_recv(Buf, Len);
}
