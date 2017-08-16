#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include "stm32f1xx_hal.h"
#include "stdint.h"
typedef enum{
	E_OK,
	E_SYSTEM_CLOCK,
	E_UART,
	E_IWDG,
	E_FEEDIWDG,
	E_RTC,
	E_RTCALARM,
	E_ADCDMA,
	E_IIC,
}ERROR_STUS;
typedef enum{
	FALSE,
	TRUE = !FALSE
}BOOLEAN;
typedef struct{
	BOOLEAN buttonFlag;
	BOOLEAN ledPwmFlag;
	BOOLEAN tim2Flag;
	BOOLEAN tim3Flag;
	BOOLEAN tim4Flag;
	BOOLEAN tim5Flag;
	BOOLEAN tim6Flag;
	BOOLEAN tim7Flag;
	BOOLEAN tim8Flag;
	BOOLEAN RTCFlag;
	BOOLEAN iicFlag;
	uint8_t usbFlag;
	uint8_t fatsFlag;
}MACHINE_STATE;
extern MACHINE_STATE machineState;
#endif
