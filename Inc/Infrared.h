#ifndef _INFRARED_H_
#define _INFRARED_H_
#include "stm32f1xx_hal.h"
#define INFRARED_DATA_IN                (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1))
#define IRDA_ID            0
extern uint32_t frame_data;    /* 一帧数据缓存 */
extern uint8_t  frame_cnt;
extern uint8_t  frame_flag;    /* 一帧数据接收完成标志 */
extern uint8_t isr_cnt;
void initInfrared(void);
uint8_t getPulseTime(void);
void infraredTest(void);
#endif
