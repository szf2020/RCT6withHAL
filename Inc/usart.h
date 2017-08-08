#ifndef __usart_H
#define __usart_H

#include "stm32f1xx_hal.h"

void uartEnableRx(void);
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);

#endif /*__ usart_H */
