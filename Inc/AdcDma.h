#ifndef _ADCDMA_H_
#define _ADCDMA_H_
#include "stm32f1xx_hal.h"

//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define V25  0x6EE
//斜率 每摄氏度4.3mV 对应每摄氏度0x05
#define AVG_SLOPE 0x05 

#define ADCChanel 2

extern uint16_t ADC_ConvertedValue[ADCChanel];

ERROR_STUS initADC1(void);
float checkChipTemp(void);
float ADCPA1transform(void);
#endif
