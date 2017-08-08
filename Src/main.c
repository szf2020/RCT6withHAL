/*
funName	:
input		:
output	:
describe:
remark	:
*/
#include "usart.h"
#include "power.h"
#include "Led.h"
#include "SystemConfig.h"

int main(void)
{
	setPriorityGroup();
	systemClockConfig();
	rccClockEnable();
	initUart(USART1);
	initLed();
  /* last set */
  setPriority();
	/* pre init set*/
  while (1)
  {
		printf("TX\n");
		LED1_TOGGLE();
		HAL_Delay(1000);
  }
 

}
