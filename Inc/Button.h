#ifndef _Button_H_
#define _Button_H_
#include "stm32f1xx_hal.h"

#define IsKey1Press	(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET)
#define IsKey0Press	(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5) == GPIO_PIN_RESET)

typedef struct{
	uint8_t  keyflag[2];
	uint16_t keycounter[2];
}BUTTON;

void initButton(void);
void scanKey1(void);
#endif
