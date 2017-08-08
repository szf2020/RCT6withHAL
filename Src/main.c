/*
funName	:
input		:
output	:
describe:
remark	:
*/
#include "usart.h"
#include "power.h"

int main(void)
{
	setPriorityGroup();
	systemClockConfig();
	rccClockEnable();
	initUart(USART1);
  /* last set */
  setPriority();
	/* pre init set*/
  while (1)
  {
		printf("TX\n");
		HAL_Delay(1000);
  }
 

}
