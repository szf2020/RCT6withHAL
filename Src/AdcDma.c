#include "AdcDma.h"
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
// ADת�����ֵ
uint16_t ADC_ConvertedValue[ADCChanel];
/*
funName	:startADC1AndDMA
input		:
output	:
describe:����ADC��DMA
remark	:
*/
void startADC1AndDMA(void){
	HAL_ADCEx_Calibration_Start(&hadc1);
	/* ����ADת����ʹ��DMA������ж� */
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&ADC_ConvertedValue,sizeof(ADC_ConvertedValue)); 
}
/*
funName	:initADC1
input		:
output	:ERROR_STUS
describe:��ʼ��adc
remark	:
*/
ERROR_STUS initADC1(void){
	ADC_ChannelConfTypeDef sConfig;
	// ADC��������
  hadc1.Instance = ADC1;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	if(HAL_ADC_Init(&hadc1) != HAL_OK){
		return E_ADCDMA;
	}
	
	// ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL_16;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		return E_ADCDMA;
	}
	// ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		return E_ADCDMA;
	}
	startADC1AndDMA();
	return E_OK;
}
/*
funName	:checkChipTemp
input		:
output	:float
describe:����оƬ�¶�ת�����
remark	:������׼
*/
float checkChipTemp(void){
	
	return (V25-ADC_ConvertedValue[0])/AVG_SLOPE+25;
	
}
/*
funName	:ADCPA1transform
input		:
output	:float
describe:����PA1��Ӧ��ADCͨ��1��ѹת�����
remark	:�������
*/
float ADCPA1transform(void){
	return (ADC_ConvertedValue[1] & 0xFFF)*3.3/4096;
}
/*
funName	:HAL_ADC_MspInit
input		:
output	:
describe:
remark	:GPIO��ʼ����DMA��ʼ��
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc){
	GPIO_InitTypeDef GPIO_InitStruct;
	if(hadc->Instance == ADC1){
		
		GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      while(1);
    }
		/* ����DMA */
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);

	}
}
