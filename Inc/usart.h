#ifndef __usart_H
#define __usart_H
#include "stm32f1xx_hal.h"

void initUart(USART_TypeDef *uart);
void deInitUart(USART_TypeDef *uart);
#endif /*__ usart_H */
