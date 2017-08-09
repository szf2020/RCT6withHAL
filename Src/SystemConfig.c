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

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
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
	__HAL_RCC_USART1_CLK_DISABLE();
	__HAL_RCC_TIM1_CLK_DISABLE();
	
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();

}
/*
funName	:setPriority
input		:NA
output	:NA
describe:���������ж����ȼ�
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
	
	/* EXTI interrupt init:KEY0(PC5)�ⲿ�ж�*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	/* ���ö�ʱ���ж����ȼ���ʹ�� */
	HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_UP_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_TRG_COM_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);
	
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	
}
/*
funName	:setPriorityGroup
input		:NA
output	:NA
describe:�������ȼ���
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
