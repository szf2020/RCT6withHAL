#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32f1xx_hal.h"

#define PWM_MAX	0x200
typedef enum{
	SPEED0,
	SPEED1,
	SPEED2,
	SPEED3,
	SPEED4,
	SPEED5,
	SPEED6,
}PWM_SPEED;

void initTimer(TIM_TypeDef *timer);
void ledPwm(void);
#endif
