#ifndef _POWER_H_
#define _POWER_H_
#include "stm32f1xx_hal.h"
ERROR_STUS systemClockConfig(void);
void rccClockDisable(void);
void rccClockEnable(void);
void enterStandbyMode(void);
void setPriority(void);
void setPriorityGroup(void);
#endif
