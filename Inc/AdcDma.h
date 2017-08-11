#ifndef _ADCDMA_H_
#define _ADCDMA_H_
#include "stm32f1xx_hal.h"

//����12λ��ADC��3.3V��ADCֵΪ0xfff,�¶�Ϊ25��ʱ��Ӧ�ĵ�ѹֵΪ1.43V��0x6EE
#define V25  0x6EE
//б�� ÿ���϶�4.3mV ��Ӧÿ���϶�0x05
#define AVG_SLOPE 0x05 

#define ADCChanel 2

extern uint16_t ADC_ConvertedValue[ADCChanel];

ERROR_STUS initADC1(void);
float checkChipTemp(void);
float ADCPA1transform(void);
#endif
