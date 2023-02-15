#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_
#include "hal_sys.h"

#define HAL_KEY_SHAKE_TIME						20//20ms祛抖时间
#define HAL_KEY_LONG_PRESS_TIME				500//500ms长按时间

/*io pin config*/
#define HAL_KEY_PORT  GPIOB->IDR
#define HAL_KEY_SW_1	GPIO_Pin_12
#define HAL_KEY_SW_ALL (HAL_KEY_SW_1)

#define HAL_KEY_POWER									BV(0)
#define HAL_KEY_ALL										(HAL_KEY_POWER)

void HalKeyInit(void);
void HalKeyTask(void);
FlagStatus HalKeyGetShortPress(u8 sw);
FlagStatus HalKeyGetLongPress(u8 sw);
u8 HalKeyGetShortPressNoClear(u8 sw);
u8 HalKeyGetLongPressNoClear(u8 sw);

#endif
