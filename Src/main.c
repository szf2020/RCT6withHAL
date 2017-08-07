/*
funName	:
input		:
output	:
describe:
remark	:
*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "BSP.h"

int main(void)
{
	BSP_RccClockEnable();
	BSP_SetPriorityGroup();
	BSP_SystemClockConfig();
	BSP_Usart1Init(115200);
  /* last set */
  BSP_SetPriority();
	/* pre init set*/
  while (1)
  {
		printf("TX\n");
		HAL_Delay(1000);
  }
 

}
