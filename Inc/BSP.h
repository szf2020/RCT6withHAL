#ifndef _BSP_H_
#define _BSP_H_
#include "stm32f1xx_hal.h"
#include "usart.h"

typedef enum{
	E_OK,
	E_SYSTEM_CLOCK,
	E_USART1
}BSP_ERROR_STUS;

void BSP_SystemClockConfig(void);
void BSP_RccClockEnable(void);
void BSP_RccClockDisable(void);
void BSP_Usart1Init(uint32_t baud);
void BSP_SetPriority(void);
void BSP_SetPriorityGroup(void);

#endif
