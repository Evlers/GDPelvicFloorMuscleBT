#ifndef _BLE_H_
#define _BLE_H_
#include "hal_sys.h"


#define BLE_USART_PORT		USART_PORT2
#define BLE_RCC_GPIO			RCC_APB2Periph_GPIOB
#define BLE_GPIO_PORT			GPIOB

#define BLE_WAKE_PIN			GPIO_Pin_11
#define BLE_WAKE_SET			BLE_GPIO_PORT->BSRR |= BLE_WAKE_PIN
#define BLE_WAKE_CLR			BLE_GPIO_PORT->BRR |= BLE_WAKE_PIN

#define BLE_RESET_PIN			GPIO_Pin_13
#define BLE_RESET_SET			BLE_GPIO_PORT->BSRR |= BLE_RESET_PIN
#define BLE_RESET_CLR			BLE_GPIO_PORT->BRR |= BLE_RESET_PIN

#define BLE_RELOAD_PIN		GPIO_Pin_14
#define BLE_RELOAD_SET		BLE_GPIO_PORT->BSRR |= BLE_RELOAD_PIN
#define BLE_RELOAD_CLR		BLE_GPIO_PORT->BRR |= BLE_RELOAD_PIN

#define BLE_NAME					"Medical"
#define VERSION						"V1.00"

void ble_init(void);
void ble_task(void);
void ble_wake(void);
void ble_sleep(void);
void ble_send(u8 *data, u16 len);
#endif
