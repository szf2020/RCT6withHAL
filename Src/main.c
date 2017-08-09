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
#include "Button.h"
#include "Timer.h"
MACHINE_STATE machineState;
ERROR_STUS errorStus = E_OK;
int main(void)
{
	errorStus = setPriorityGroup();
	errorStus = systemClockConfig();
	
	rccClockEnable();
	
	initUart(USART1);
	initLed();
	initButton();
	initTimer(TIM1);
	initTimer(TIM2);
	initTimer(TIM3);
	initTimer(TIM4);
	initTimer(TIM5);
	initTimer(TIM6);
	initTimer(TIM7);
	initTimer(TIM8);
  /* last set */
	//initIwdg();
  setPriority();
	/* pre init set*/
	checkPowerMode();
	//enterSleepMode();
  while (1)
  {
		scanKey1();
		ledPwm();
		//timerTest();
		//feedIwdg();
  }
 

}
