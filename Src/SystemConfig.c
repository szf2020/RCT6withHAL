#include "SystemConfig.h"
/*
funName	:SystemClock_Config
input		:NA
output	:ERROR_STUS
describe:init system clock
remark	:
sysclock 72M
APB1:36M
APB2:72M
*/
ERROR_STUS systemClockConfig(void)
{
	
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return E_SYSTEM_CLOCK;
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    return E_SYSTEM_CLOCK;
  }

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK){
		return E_SYSTEM_CLOCK;
	}
	
    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	return E_OK;
}
/*
funName	:rccClockEnable
input		:NA
output	:NA
describe:enable RCC CLOCK
remark	:
*/
void rccClockEnable(void)
{
	__HAL_RCC_AFIO_CLK_ENABLE();
	
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM5_CLK_ENABLE();
	__HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_TIM7_CLK_ENABLE();
	__HAL_RCC_TIM8_CLK_ENABLE();
	
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();
}
/*
funName	:rccClockDisable
input		:NA
output	:NA
describe:disable RCC CLOCK
remark	:
*/
void rccClockDisable(void)
{
	__HAL_RCC_AFIO_CLK_DISABLE();
	
	__HAL_RCC_TIM1_CLK_DISABLE();
	__HAL_RCC_TIM2_CLK_DISABLE();
	__HAL_RCC_TIM3_CLK_DISABLE();
	__HAL_RCC_TIM4_CLK_DISABLE();
	__HAL_RCC_TIM5_CLK_DISABLE();
	__HAL_RCC_TIM6_CLK_DISABLE();
	__HAL_RCC_TIM7_CLK_DISABLE();
	__HAL_RCC_TIM8_CLK_DISABLE();
	
  __HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	
	__HAL_RCC_ADC1_CLK_DISABLE();
	__HAL_RCC_SPI1_CLK_DISABLE();
	__HAL_RCC_USART1_CLK_DISABLE();
}
/*
funName	:setPriority
input		:NA
output	:NA
describe:设置所有中断优先级
remark	:
*/
void setPriority(void)
{
  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
	/* Peripheral interrupt init */
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	
	/* EXTI interrupt init:KEY0(PC5)外部中断*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	/* 配置定时器中断优先级并使能 */
	HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_UP_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_TRG_COM_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 13, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 12, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM4_IRQn, 11, 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	HAL_NVIC_SetPriority(TIM5_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
	HAL_NVIC_SetPriority(TIM6_IRQn, 9, 0);
	HAL_NVIC_EnableIRQ(TIM6_IRQn);
	HAL_NVIC_SetPriority(TIM7_IRQn, 8, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
	
	HAL_NVIC_SetPriority(TIM8_BRK_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(TIM8_BRK_IRQn);
	HAL_NVIC_SetPriority(TIM8_UP_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);
	HAL_NVIC_SetPriority(TIM8_TRG_COM_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(TIM8_TRG_COM_IRQn);
	HAL_NVIC_SetPriority(TIM8_CC_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
	
}
/*
funName	:setPriorityGroup
input		:NA
output	:NA
describe:设置优先级组
remark	:
*/
void setPriorityGroup(void)
{
  /* Configure Flash prefetch */
#if (PREFETCH_ENABLE != 0)
#if defined(STM32F101x6) || defined(STM32F101xB) || defined(STM32F101xE) || defined(STM32F101xG) || \
    defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || defined(STM32F103xE) || defined(STM32F103xG) || \
    defined(STM32F105xC) || defined(STM32F107xC)

  /* Prefetch buffer is not available on value line devices */
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif
#endif /* PREFETCH_ENABLE */
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}
