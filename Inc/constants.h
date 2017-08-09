#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
typedef enum{
	E_OK,
	E_SYSTEM_CLOCK,
	E_UART
}ERROR_STUS;
typedef enum{
	FALSE,
	TRUE = !FALSE
}BOOLEAN;
typedef struct{
	BOOLEAN buttonFlag;
	BOOLEAN ledPwmFlag;
}MACHINE_STATE;
#endif
