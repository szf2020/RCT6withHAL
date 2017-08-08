#include "Led.h"

void initLed(void){
	GPIO_InitTypeDef GPIO_InitStruct;
 /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	LED1_OFF();
}
