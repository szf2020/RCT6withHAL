#ifndef _Led_H_
#define _Led_H_
#include "stm32f1xx_hal.h"

#define LED1_ON() 		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET)
#define LED1_OFF() 		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET)
#define LED1_TOGGLE() HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_2)

void initLed(void);

#endif
