#ifndef _POWER_H_
#define _POWER_H_
#include "stm32f1xx_hal.h"

void enterStandbyMode(void);
ERROR_STUS initIwdg(void);
ERROR_STUS feedIwdg(void);

#endif
