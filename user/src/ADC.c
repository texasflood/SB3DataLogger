
#include "stm32F10x.h"
#include "ADC.h"
#include "UART.h"
#include "LEDs.h"
#include "TIM.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C)

extern __IO uint32_t ADC_DualConvertedValueTab[16];
extern volatile uint8_t new_data;

// ADC Buffer Definitions
#define ADC_BUFFER_SIZE_2 100

uint16_t ADC_results_2[ADC_BUFFER_SIZE_2];
int ADC_count_2 = 0;
uint16_t ADCVal = 0;

//Private function prototypes
void ADC_Config(void);
void ADC_RCC_Config(void);
void ADC_GPIO_Config(void);
void ADC_NVIC_Config(void);

//------------------------------------------------------------------------

//Initialise ADC for independent sampling on demand
void ADC_init(void)
{
	//Enable required clocks
	ADC_RCC_Config();
	
	//Configure required pin as analogue input
	ADC_GPIO_Config();
	
	//Configure the ADC itself
	ADC_Config();
	
	//Configure NVIC and ADC interrupt to trigger on End of Conversion (EOC)
	ADC_NVIC_Config();
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

uint16_t ADC_perform_single_conversion(void)
{
	uint16_t ADC_word;
	
	//Clear end of conversion flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	//Start conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//Wait for conversion to complete
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	//Get value
	ADC_word = ADC_GetConversionValue(ADC1);
	
	return ADC_word;
}

//------------------------------------------------------------------------

void ADC_Config(void)
{
	int i;
	ADC_InitTypeDef ADC_InitStructure;	//Structure to hold ADC config parameters
	
	//Set ADC config parameters
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//Independent mode - ADC1 and ADC2 operate independently
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//Only perform a single conversion (don't scan a range of input pins)
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//Sample only on external signal, not continuously
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//Don't enable any automatic external sampling trigger
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;	//**NEW - enable external trigger from timer 3 update**
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//Align 12-bit samples to the right of their destination word in memory
  ADC_InitStructure.ADC_NbrOfChannel = 1;	//One conversion channel (scan mode would use multiple channels)
  ADC_Init(ADC1, &ADC_InitStructure);
  
	//**NEW - ENABLE EXTERNAL TRIGGERING*
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	
	//ADC1 regular channels configuration (just one channel)
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);    
	
	//power up ADC
	ADC_Cmd(ADC1, ENABLE);
	
	//wait until ADC is ready
	for (i=0; i<10000; i++);
}

void ADC_RCC_Config(void)
{
	//Set ADC clock divider
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
	
	//Enable DMA periph clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//Enable ADC and GPIOC periph clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
}

void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Set PC0 as analogue input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC1_IRQHandler(void)
{
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	
	//ADC_results_2[ADC_count_2++] = ADC_GetConversionValue(ADC1);
	ADCVal = ADC_GetConversionValue(ADC1);
	
	DAC_SetChannel1Data(DAC_Align_12b_R, ADCVal);
	
	/*if (ADC_count_2 > ADC_BUFFER_SIZE_2)
	{
		ADC_count_2 = 0;
	}*/
	
	LED_toggle();
}

void ADC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}


