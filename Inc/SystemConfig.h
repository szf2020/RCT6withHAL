#ifndef _SystemConfig_H_
#define _SystemConfig_H_
#include "stm32f1xx_hal.h"

ERROR_STUS systemClockConfig(void);
void rccClockDisable(void);
void rccClockEnable(void);
void setPriority(void);
ERROR_STUS setPriorityGroup(void);

#endif
